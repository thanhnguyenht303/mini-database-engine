#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

constexpr std::size_t PAGE_SIZE = 4096;
constexpr std::size_t MAX_PAGES = 100;

class Pager {
    public:
        explicit Pager(const std::string& filename);
        ~Pager();

        std::array<char, PAGE_SIZE>& get_page(std::size_t page_num);
        void flush_page(std::size_t page_num);
        std::size_t num_pages() const;
        std::size_t allocate_page();

    private:
        std::fstream file_;
        std::vector<std::array<char, PAGE_SIZE>> pages_;
        std::vector<bool> loaded_;
        std::size_t file_length_;
};