#include "table.hpp"

#include <stdexcept>
#include <algorithm>

namespace {
    std::vector<Row> rows_from_leaf(const std::array<char, PAGE_SIZE>& page) {
        if (get_node_type(page) != NodeType::leaf) {
            throw std::runtime_error("page is not a leaf node");
        }

        std::uint32_t count = leaf_node_num_cells(page);

        std::vector<Row> rows;
        rows.reserve(count);

        for(std::uint32_t i = 0; i < count; ++i){
            rows.push_back(leaf_node_value(page, i));
        }

        return rows;
    }

    std::optional<Row> find_in_leaf(const std::array<char, PAGE_SIZE>& page, std::uint64_t id){
        if(get_node_type(page) != NodeType::leaf){
            throw std::runtime_error("page is not leaf node");
        }

        std::uint32_t left = 0;
        std::uint32_t right = leaf_node_num_cells(page);

        while(left < right) {
            std::uint32_t mid = left + (right - left)/2;
            std::uint64_t key = leaf_node_key(page, mid);

            if(key == id) {
                return leaf_node_value(page, mid);
            }
            else if(key < id){
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }

        return std::nullopt;
    }
}

Table::Table(const std::string& filename) : pager_(filename) {
    if(pager_.num_pages() == 0) {
        auto& root_page = pager_.get_page(0);
        initialize_leaf_node(root_page, true);
        pager_.flush_page(0);
    }
}

void Table::insert(const Row& row) {
    auto& root = pager_.get_page(0);
    

    if(get_node_type(root) == NodeType::leaf){
        if(leaf_node_num_cells(root) < LEAF_NODE_MAX_CELLS) {
            leaf_node_insert_nonfull(root, row);
            pager_.flush_page(0);
        }
        else {
            split_root_leaf_and_insert(row);
        }
    }
    else {
        throw std::runtime_error("internal root insert not implemented yet");
    }
}

std::vector<Row> Table::select_all() {
    auto& root = pager_.get_page(0);

    if(get_node_type(root) == NodeType::leaf){
        return rows_from_leaf(root);
    }
    else if(get_node_type(root) == NodeType::internal){
        std::uint32_t left_page_num = internal_node_child(root, 0);
        std::uint32_t right_page_num = internal_node_right_child(root);

        auto& left_page = pager_.get_page(left_page_num);
        auto& right_page = pager_.get_page(right_page_num);

        auto left_rows = rows_from_leaf(left_page);
        auto right_rows = rows_from_leaf(right_page);

        left_rows.insert(left_rows.end(), right_rows.begin(), right_rows.end());
        return left_rows;
    }
    else {
        throw std::runtime_error("unknown root node type");
    }
}

std::size_t Table::row_count() {
    auto& page = pager_.get_page(0);
    
    if(get_node_type(page) != NodeType::leaf){
        throw std::runtime_error("root page is not a leaf node");
    }

    return leaf_node_num_cells(page);
}

std::optional<Row> Table::find(std::uint64_t id) {
    auto& root = pager_.get_page(0);

    if(get_node_type(root) == NodeType::leaf){
        return find_in_leaf(root, id);
    }
    else if(get_node_type(root) == NodeType::internal){
        std::uint64_t separator = internal_node_key(root, 0);
        std::uint32_t target;
        if(id <= separator){
            target = internal_node_child(root, 0);
        }
        else {
            target = internal_node_right_child(root);
        }

        auto& target_page = pager_.get_page(target);
        return find_in_leaf(target_page, id);

    }
    else {
        throw std::runtime_error("unknown root node type");
    }
}

void Table::split_root_leaf_and_insert(const Row& row) {
    auto& root = pager_.get_page(0);

    if(get_node_type(root) != NodeType::leaf){
        throw std::runtime_error("root is not a leaf");
    }

    std::uint32_t old_count = leaf_node_num_cells(root);

    if(old_count < LEAF_NODE_MAX_CELLS) {
        throw std::runtime_error("root leaf is not full");
    }

    std::vector<Row> all_rows;
    all_rows.reserve(old_count + 1);

    for(std::uint32_t i = 0; i < old_count; ++i) {
        all_rows.push_back(leaf_node_value(root, i));
    }

    for(const auto existing : all_rows){
        if(existing.id == row.id){
            throw std::runtime_error("duplicate id");
        }
    }

    all_rows.push_back(row);

    std::sort(all_rows.begin(), all_rows.end(),
        [](const Row& a, const Row& b){
            return a.id < b.id;
        }
    );

    const std::size_t total = all_rows.size();
    const std::size_t left_count = total / 2;
    const std::size_t right_count = total - left_count;

    std::size_t left_page_num = pager_.allocate_page();
    std::size_t right_page_num = pager_.allocate_page();

    auto& left_page = pager_.get_page(left_page_num);
    auto& right_page = pager_.get_page(right_page_num);

    initialize_leaf_node(left_page, false);
    initialize_leaf_node(right_page, false);

    for(std::size_t i = 0; i < left_count; ++i) {
        leaf_node_insert_nonfull(left_page, all_rows[i]);
    }

    for(std::size_t i = 0; i < right_count; ++i) {
        leaf_node_insert_nonfull(right_page, all_rows[left_count + i]);
    }

    initialize_internal_node(root, true);

    set_internal_node_cell(root, 0, static_cast<std::uint32_t>(left_page_num), leaf_node_max_key(left_page));
    set_internal_node_right_child(root, static_cast<std::uint32_t>(right_page_num));
    set_internal_node_num_keys(root, 1);

    pager_.flush_page(left_page_num);
    pager_.flush_page(right_page_num);
    pager_.flush_page(0);
}