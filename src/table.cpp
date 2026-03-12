#include "table.hpp"

#include <array>
#include <cstring>
#include <stdexcept>

namespace {
    std::uint32_t read_row_count(const std::array<char, PAGE_SIZE>& page) {
        std::uint32_t count = 0;
        std::memcpy(&count, page.data(), sizeof(count));
        return count;
    }

    void write_row_count(std::array<char, PAGE_SIZE>& page, std::uint32_t count) {
        std::memcpy(page.data(), &count, sizeof(count));
    }

    std::size_t row_offset(std::size_t index){
        return TABLE_HEADER_SIZE + index * ROW_SIZE;
    }
}

Table::Table(const std::string& filename) : pager_(filename) {}

void Table::insert(const Row& row) {
    auto& page = pager_.get_page(0);
    std::uint32_t count = read_row_count(page);

    if(count >= ROWS_PER_PAGE) {
        throw std::runtime_error("table full");
    }

    for (std::uint32_t i = 0; i < count; ++i) {
        std::array<char, ROW_SIZE> raw_row{};
        std::memcpy(raw_row.data(), page.data() + row_offset(i), ROW_SIZE);
        Row existing = deserialize_row(raw_row);

        if(existing.id == row.id){
            throw std::runtime_error("duplicate id");
        }
    }

    std::array<char, ROW_SIZE> raw_row{};
    serialize_row(row, raw_row);

    std::memcpy(page.data() + row_offset(count), raw_row.data(), ROW_SIZE);
    write_row_count(page, count + 1);
    pager_.flush_page(0);
}

std::vector<Row> Table::select_all() {
    auto& page = pager_.get_page(0);
    std::uint32_t count = read_row_count(page);

    std::vector<Row> rows;
    rows.reserve(count);

    for (std::uint32_t i = 0; i < count; ++i){
        std::array<char, ROW_SIZE> raw_row{};
        std::memcpy(raw_row.data(), page.data() + row_offset(i), ROW_SIZE);
        rows.push_back(deserialize_row(raw_row));
    }

    return rows;
}

std::size_t Table::row_count() {
    auto& page = pager_.get_page(0);
    return read_row_count(page);
}