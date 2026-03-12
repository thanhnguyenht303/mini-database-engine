#include "row.hpp"

#include <array> 
#include <cassert>

int main() {
    Row row{1, "alice", "alice@example.com"};
    std::array<char, ROW_SIZE> data{};

    serialize_row(row, data);
    Row out = deserialize_row(data);

    assert(out.id == 1);
    assert(out.username == "alice");
    assert(out.email == "alice@example.com");

    return 0;
}