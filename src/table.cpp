#include "table.hpp"

#include <stdexcept>

Table::Table(const std::string& filename) : pager_(filename) {
    if(pager_.num_pages() == 0) {
        auto& root_page = pager_.get_page(0);
        initialize_leaf_node(root_page, true);
        pager_.flush_page(0);
    }
}

void Table::insert(const Row& row) {
    auto& page = pager_.get_page(0);
    

    if(get_node_type(page) != NodeType::leaf){
        throw std::runtime_error("root page is not leaf node");
    }

    leaf_node_insert(page, row);
    pager_.flush_page(0);
}

std::vector<Row> Table::select_all() {
    auto& page = pager_.get_page(0);
    
    if(get_node_type(page) != NodeType::leaf){
        throw std::runtime_error("root page is not a leaf node");
    }

    std::uint32_t count = leaf_node_num_cells(page);

    std::vector<Row> rows;
    rows.reserve(count);

    for(std::uint32_t i = 0; i < count; ++i) {
        rows.push_back(leaf_node_value(page, i));
    }    

    return rows;
}

std::size_t Table::row_count() {
    auto& page = pager_.get_page(0);
    
    if(get_node_type(page) != NodeType::leaf){
        throw std::runtime_error("root page is not a leaf node");
    }

    return leaf_node_num_cells(page);
}