#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

struct Row {
    std::uint64_t id;
    std::string username;
    std::string email;
};

constexpr std::size_t USERNAME_SIZE = 32;
constexpr std::size_t EMAIL_SIZE = 256;
constexpr std::size_t ROW_SIZE = sizeof(std::uint64_t) + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(const Row& row, std::array<char, ROW_SIZE>& out);
Row deserialize_row(const std::array<char, ROW_SIZE>& in);