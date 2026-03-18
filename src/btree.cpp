#include "btree.hpp"

#include <cstring>
#include <stdexcept>

namespace {
    std::size_t leaf_node_cell_offset(std::size_t cell_num) {
        return LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
    }

    std::size_t leaf_node_key_offset(std::size_t cell_num) {
        return leaf_node_cell_offset(cell_num);
    }

    std::size_t leaf_node_value_offset(std::size_t cell_num) {
        return leaf_node_cell_offset(cell_num) + LEAF_NODE_KEY_SIZE;
    }

    std::size_t internal_node_cell_offset(std::size_t cell_num) {
        return INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
    }

    std::size_t internal_node_child_offset(std::size_t cell_num) {
        return internal_node_cell_offset(cell_num);
    }

    std::size_t internal_node_key_offset(std::size_t cell_num) {
        return internal_node_cell_offset(cell_num) + INTERNAL_NODE_CHILD_SIZE;
    }
}

void initialize_leaf_node(std::array<char, PAGE_SIZE>& page, bool is_root) {
    page.fill(0);

    const std::uint8_t type = static_cast<std::uint8_t>(NodeType::leaf);
    const std::uint8_t root = is_root ? 1 : 0;
    const std::uint32_t count = 0;

    std::memcpy(page.data() + NODE_TYPE_OFFSET, &type, NODE_TYPE_SIZE);
    std::memcpy(page.data() + IS_ROOT_OFFSET, &root, IS_ROOT_SIZE);
    std::memcpy(page.data() + NUM_CELLS_OFFSET, &count, NUM_CELLS_SIZE);
}

void initialize_internal_node(std::array<char, PAGE_SIZE>& page, bool is_root){
    page.fill(0);

    const std::uint8_t type = static_cast<std::uint8_t>(NodeType::internal);
    const std::uint8_t root = is_root ? 1 : 0;
    const std::uint32_t num_keys = 0;
    const std::uint32_t right_child = 0;

    std::memcpy(page.data() + NODE_TYPE_OFFSET, &type, NODE_TYPE_SIZE);
    std::memcpy(page.data() + IS_ROOT_OFFSET, &root, IS_ROOT_SIZE);
    std::memcpy(page.data() + INTERNAL_NODE_NUM_KEYS_OFFSET, &num_keys, sizeof(num_keys));
    std::memcpy(page.data() + INTERNAL_NODE_RIGHT_CHILD_OFFSET, &right_child, sizeof(right_child));
}

NodeType get_node_type(const std::array<char, PAGE_SIZE>& page) {
    std::uint8_t type = 0;
    std::memcpy(&type, page.data() + NODE_TYPE_OFFSET, NODE_TYPE_SIZE);
    return static_cast<NodeType>(type);
}

std::uint32_t leaf_node_num_cells(const std::array<char, PAGE_SIZE>& page){
    std::uint32_t count = 0;
    std::memcpy(&count, page.data() + NUM_CELLS_OFFSET, NUM_CELLS_SIZE);
    return count;
}

void set_leaf_node_num_cells(std::array<char, PAGE_SIZE>& page, std::uint32_t count) {
    std::memcpy(page.data() + NUM_CELLS_OFFSET, &count, NUM_CELLS_SIZE);
}

std::uint64_t leaf_node_key(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num) {
    std::uint64_t key = 0;
    std::memcpy(&key, page.data() + leaf_node_key_offset(cell_num), LEAF_NODE_KEY_SIZE);
    return key;
}

Row leaf_node_value(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num) {
    std::array<char, ROW_SIZE> raw{};
    std::memcpy(raw.data(), page.data() + leaf_node_value_offset(cell_num), ROW_SIZE);
    return deserialize_row(raw);
}

void leaf_node_insert(std::array<char, PAGE_SIZE>& page, const Row& row) {
    std::uint32_t count = leaf_node_num_cells(page);

    if(count >= LEAF_NODE_MAX_CELLS) {
        throw std::runtime_error("leaf node full");
    }

    leaf_node_insert_nonfull(page, row);
}

std::uint32_t internal_node_child(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num){
    std::uint32_t child = 0;
    std::memcpy(&child, page.data() + internal_node_child_offset(cell_num), sizeof(child));
    return child;
}

std::uint64_t internal_node_key(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num) {
    std::uint64_t key = 0;
    std::memcpy(&key, page.data() + internal_node_key_offset(cell_num), sizeof(key));
    return key;
}

std::uint32_t internal_node_num_keys(const std::array<char, PAGE_SIZE>& page){
    std::uint32_t num_keys = 0;
    std::memcpy(&num_keys, page.data() + INTERNAL_NODE_NUM_KEYS_OFFSET, sizeof(num_keys));
    return num_keys;
}

void set_internal_node_num_keys(std::array<char, PAGE_SIZE>& page, std::uint32_t count) {
    std::memcpy(page.data() + INTERNAL_NODE_NUM_KEYS_OFFSET, &count, sizeof(count));
}

std::uint32_t internal_node_right_child(const std::array<char, PAGE_SIZE>& page) {
    std::uint32_t right_child = 0;
    std::memcpy(&right_child, page.data() + INTERNAL_NODE_RIGHT_CHILD_OFFSET, sizeof(right_child));
    return right_child;
}

void set_internal_node_right_child(std::array<char, PAGE_SIZE>& page, std::uint32_t page_num) {
    std::memcpy(page.data() + INTERNAL_NODE_RIGHT_CHILD_OFFSET, &page_num, sizeof(page_num));
}

void set_internal_node_cell(std::array<char, PAGE_SIZE>& page, std::size_t cell_num, std::uint32_t child_page, std::uint64_t key){
    std::memcpy(page.data() + internal_node_child_offset(cell_num), &child_page, sizeof(child_page));
    std::memcpy(page.data() + internal_node_key_offset(cell_num), &key, sizeof(key));
}

std::uint64_t leaf_node_max_key(const std::array<char, PAGE_SIZE>& page) {
    std::uint32_t count = leaf_node_num_cells(page);
    if(count == 0){
        throw std::runtime_error("leaf node is empty");
    }

    return leaf_node_key(page, count - 1);
}

void leaf_node_insert_nonfull(std::array<char, PAGE_SIZE>& page, const Row& row){
    std::uint32_t count = leaf_node_num_cells(page);

    std::size_t index = 0;
    while (index < count && leaf_node_key(page, index) < row.id) {
        ++index;
    }

    if(index < count && leaf_node_key(page, index) == row.id) {
        throw std::runtime_error("duplicate id");
    }

    for (std::size_t i = count; i > index; --i){
        std::memmove(
            page.data() + leaf_node_cell_offset(i),
            page.data() + leaf_node_cell_offset(i-1),
            LEAF_NODE_CELL_SIZE
        );
    }

    std::array<char, ROW_SIZE> raw{};
    serialize_row(row, raw);

    std::memcpy(page.data() + leaf_node_key_offset(index), &row.id, LEAF_NODE_KEY_SIZE);
    std::memcpy(page.data() + leaf_node_value_offset(index), raw.data(), ROW_SIZE);

    set_leaf_node_num_cells(page, count + 1);
}