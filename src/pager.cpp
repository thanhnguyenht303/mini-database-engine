#include "pager.hpp"

#include <stdexcept>

Pager::Pager(const std::string& filename)
    : pages_(MAX_PAGES), loaded_(MAX_PAGES, false), file_length_(0){
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if(!file_.is_open()) {
        std::ofstream create_file(filename, std::ios::binary);
        create_file.close();
        file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    if(!file_.is_open()) {
        throw std::runtime_error("could not open database file");
    }

    file_.seekg(0, std::ios::end);
    file_length_ = static_cast<std::size_t>(file_.tellg());
    file_.seekg(0, std::ios::beg);
}

Pager::~Pager() {
    for (std::size_t i = 0; i < MAX_PAGES; ++i) {
        if (loaded_[i]){
            flush_page(i);
        }
    }
    file_.close();
}

std::array<char, PAGE_SIZE>& Pager::get_page(std::size_t page_num){
    if (page_num >= MAX_PAGES){
        throw std::runtime_error("page number out of bound");
    }

    if(!loaded_[page_num]) {
        pages_[page_num].fill(0);
        
        std::size_t offset = page_num * PAGE_SIZE;
        if (offset < file_length_) {
            file_.seekg(offset, std::ios::beg);
            file_.read(pages_[page_num].data(), PAGE_SIZE);
        }

        loaded_[page_num] = true;
    }

    return pages_[page_num];
}

void Pager::flush_page(std::size_t page_num){
    if (page_num >= MAX_PAGES){
        throw std::runtime_error("page number out of bounds");
    }

    if(!loaded_[page_num]){
        return;
    }

    std::size_t offset = page_num * PAGE_SIZE;
    file_.seekp(offset, std::ios::beg);
    file_.write(pages_[page_num].data(), PAGE_SIZE);
    file_.flush();

    if(offset + PAGE_SIZE > file_length_) {
        file_length_ = offset + PAGE_SIZE;
    }
}

std::size_t Pager::num_pages() const {
    return (file_length_ + PAGE_SIZE - 1) / PAGE_SIZE;
}

std::size_t Pager::allocate_page() {
    std::size_t page_num = num_pages();

    if(page_num >= MAX_PAGES) {
        throw std::runtime_error("database full");
    }

    auto& page = get_page(page_num);
    page.fill(0);
    loaded_[page_num] = true;
    flush_page(page_num);

    return page_num;
}