#include "row.hpp"

#include <cstring>
#include <stdexcept>

namespace {
    std::string read_fixed_string(const char* data, std::size_t max_len){
        std::size_t len = 0;
        while (len < max_len && data[len] != '\0'){
            ++len;
        }
        return std::string(data, len);
    }
}

void serialize_row(const Row& row, std::array<char, ROW_SIZE>& out) {
    if (row.username.size() > USERNAME_SIZE){
        throw std::runtime_error("username too long");
    }

    if(row.email.size() > EMAIL_SIZE){
        throw std::runtime_error("email too long");
    }

    out.fill(0);

    std::size_t offset = 0;

    std::memcpy(out.data() + offset, &row.id, sizeof(row.id));
    offset += sizeof(row.id);

    std::memcpy(out.data() + offset, row.username.data(), row.username.size());
    offset += USERNAME_SIZE;

    std::memcpy(out.data() + offset, row.email.data(), row.email.size());
}

Row deserialize_row(const std::array<char, ROW_SIZE>& in){
    Row row{};

    std::size_t offset = 0;
    
    std::memcpy(&row.id, in.data() + offset, sizeof(row.id));
    offset += sizeof(row.id);

    row.username = read_fixed_string(in.data() + offset, USERNAME_SIZE);
    offset += USERNAME_SIZE;

    row.email = read_fixed_string(in.data() + offset, EMAIL_SIZE);

    return row;
}