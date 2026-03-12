#include "table.hpp"

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <string>

int main() {
    const std::string filename = "test_table.db";
    std::remove(filename.c_str());

    {
        Table table(filename);

        table.insert(Row{1, "alice", "alice@example.com"});
        table.insert(Row{2, "bob", "bob@gmail.com"});

        assert(table.row_count() == 2);

        auto rows = table.select_all();
        assert(rows.size() == 2);

        assert(rows[0].id == 1);
        assert(rows[0].username == "alice");
        assert(rows[0].email == "alice@example.com");

        assert(rows[1].id == 2);
        assert(rows[1].username == "bob");
        assert(rows[1].email == "bob@gmail.com");
    }

    {
        Table table(filename);

        assert(table.row_count() == 2);

        auto rows = table.select_all();
        assert(rows.size() == 2);
        assert(rows[0].id == 1);
        assert(rows[1].id == 2);
    }

    {
        Table table(filename);

        bool threw = false;

        try {
            table.insert(Row{1, "another_test", "another_test@example.com"});
        } catch (const std::runtime_error&){
            threw = true;
        }

        assert(threw);
    }

    std::remove(filename.c_str());
    return 0;
};