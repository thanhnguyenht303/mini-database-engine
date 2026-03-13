#include "pager.hpp"
#include "row.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

enum class NodeType : std::uint8_t {
    internal = 0,
    leaf = 1
};

constexpr std::size_t NODE_TYPE_SIZE = sizeof(std::uint8_t);
constexpr std::size_t IS_ROOT_SIZE = sizeof(std::uint8_t);
constexpr std::size_t NUM_CELLS_SIZE = sizeof(std::uint32_t);

constexpr std::size_t NODE_TYPE_OFFSET = 0;
constexpr std::size_t IS_ROOT_OFFSET = NODE_TYPE_OFFSET + NODE_TYPE_SIZE;
constexpr std::size_t NUM_CELLS_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;

constexpr std::size_t LEAF_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + NUM_CELLS_SIZE;

constexpr std::size_t LEAF_NODE_KEY_SIZE = sizeof(std::uint64_t);
constexpr std::size_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
constexpr std::size_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;

constexpr std::size_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;

constexpr std::size_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

void initialize_leaf_node(std::array<char, PAGE_SIZE>& page, bool is_root);
NodeType get_node_type(const std::array<char, PAGE_SIZE>& page);
std::uint32_t leaf_node_sum_cells(const std::array<char, PAGE_SIZE>& page);
void set_leaf_node_num_cells(std::array<char, PAGE_SIZE>& page, std::uint32_t count);

std::uint64_t leaf_node_key(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num);
Row leaf_node_value(const std::array<char, PAGE_SIZE>& page, std::size_t cell_num);

void leaf_node_insert(std::array<char, PAGE_SIZE>& page, const Row& row);