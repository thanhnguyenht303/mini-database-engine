#pragma once

#include "pager.hpp"
#include "row.hpp"

#include <cstddef>
#include <string>
#include <vector>

constexpr std::size_t TABLE_HEADER_SIZE = sizeof(std::uint32_t);
constexpr std::size_t ROWS_PER_PAGE = (PAGE_SIZE - TABLE_HEADER_SIZE) / ROW_SIZE;

class Table {
    public:
        explicit Table(const std::string& filename);

        void insert(const Row& row);
        std::vector<Row> select_all();
        std::size_t row_count();

    private: 
        Pager pager_;
};