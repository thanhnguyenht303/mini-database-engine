#include "btree.hpp"

#include <array>
#include <cassert>
#include <stdexcept>
#include <string>

int main () {
    {
        // Leaf initialization
        std::array<char, PAGE_SIZE> page{};
        initialize_leaf_node(page, true);

        assert(get_node_type(page) == NodeType::leaf);
        assert(leaf_node_num_cells(page) == 0);
    }

    {
        //Internal initialization
        std::array<char, PAGE_SIZE> page{};
        initialize_internal_node(page, true);

        assert(get_node_type(page) == NodeType::internal);
        assert(internal_node_num_keys(page) == 0);
        assert(internal_node_right_child(page) == 0);
    }

    {
        //Leaf insert
        std::array<char, PAGE_SIZE> page{};
        initialize_leaf_node(page, true);

        leaf_node_insert_nonfull(page, Row{3, "charlie", "charlie@example.com"});
        leaf_node_insert_nonfull(page, Row{1, "alice", "alice@example.com"});
        leaf_node_insert_nonfull(page, Row{2, "bob", "bob@gmail.com"});

        assert(leaf_node_num_cells(page) == 3);
        assert(leaf_node_key(page, 0) == 1);
        assert(leaf_node_key(page, 1) == 2);
        assert(leaf_node_key(page, 2) == 3);

        Row r0 = leaf_node_value(page, 0);
        Row r1 = leaf_node_value(page, 1);
        Row r2 = leaf_node_value(page, 2);

        assert(r0.username == "alice");
        assert(r1.username == "bob");
        assert(r2.username == "charlie");
    }

    {
        //Duplicate throw
        std::array<char, PAGE_SIZE> page{};
        initialize_leaf_node(page, true);

        leaf_node_insert_nonfull(page, Row{1, "alice", "alice@example.com"});
        
        bool threw = false;
        try {
            leaf_node_insert_nonfull(page, Row{1, "duplicate", "duplicate@example.com"});
        } catch (const std::runtime_error&){
            threw = true;
        }

        assert(threw);
    }

    {
        //Return largest key
        std::array<char, PAGE_SIZE> page{};
        initialize_leaf_node(page, true);

        leaf_node_insert_nonfull(page, Row{10, "ten", "ten@example.com"});
        leaf_node_insert_nonfull(page, Row{5, "five", "five@example.com"});
        leaf_node_insert_nonfull(page, Row{7, "seven", "seven@example.com"});

        assert(leaf_node_max_key(page) == 10);
    }

    {
        //Internal node
        std::array<char, PAGE_SIZE> page{};
        initialize_internal_node(page, true);

        set_internal_node_cell(page, 0, 5, 42);
        set_internal_node_right_child(page , 8);
        set_internal_node_num_keys(page, 1);

        assert(internal_node_num_keys(page) == 1);
        assert(internal_node_child(page, 0) == 5);
        assert(internal_node_key(page, 0) == 42);
        assert(internal_node_right_child(page) == 8);
    }

    return 0;
}