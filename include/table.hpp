#pragma once

#include "btree.hpp"

#include <cstddef>
#include <string>
#include <vector>

class Table {
    public:
        explicit Table(const std::string& filename);

        void insert(const Row& row);
        std::vector<Row> select_all();
        std::size_t row_count();

    private: 
        Pager pager_;
};