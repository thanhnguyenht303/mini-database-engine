#pragma once

#include "btree.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <optional>

class Table {
    public:
        explicit Table(const std::string& filename);

        void insert(const Row& row);
        std::vector<Row> select_all();
        std::size_t row_count();
        std::optional<Row> find(std::uint64_t id);

    private: 
        Pager pager_;
        void split_root_leaf_and_insert(const Row& row);
};