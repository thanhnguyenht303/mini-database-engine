#include "table.hpp"

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <string>

using namespace std;

int main() {
    {
        const std::string filename = "test_table.db";
        std::remove(filename.c_str());

        {
            Table table(filename);

            table.insert(Row{3, "charlie", "charlie@example.com"});
            table.insert(Row{1, "alice", "alice@example.com"});
            table.insert(Row{2, "bob", "bob@gmail.com"});

            auto rows = table.select_all();
            assert(rows.size() == 3);

            assert(rows[0].id == 1);
            assert(rows[0].username == "alice");
            assert(rows[0].email == "alice@example.com");

            assert(rows[1].id == 2);
            assert(rows[1].username == "bob");
            assert(rows[1].email == "bob@gmail.com");

            assert(rows[2].id == 3);
            assert(rows[2].username == "charlie");
            assert(rows[2].email == "charlie@example.com");
        }

        {
            Table table(filename);

            auto rows = table.select_all();
            assert(rows.size() == 3);
            assert(rows[0].id == 1);
            assert(rows[1].id == 2);
            assert(rows[2].id == 3);
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

        {
            Table table(filename);

            auto found = table.find(2);
            assert(found->id == 2);
            assert(found->username == "bob");
            assert(found->email == "bob@gmail.com");

            auto missing = table.find(99);
            assert(!missing.has_value());
        }
        std::remove(filename.c_str());
    }
    

    {
        const std::string split_file_name = "full_cell_test_table.db";
        std::remove(split_file_name.c_str());

        {
            Table table(split_file_name);

            for(std::uint64_t i = 1; i <= LEAF_NODE_MAX_CELLS; ++i) {
                std::string user = "user" + to_string(i);
                table.insert(Row{i, user, user + "@example.com"});
            }

            auto rows = table.select_all();
            assert(rows.size() == LEAF_NODE_MAX_CELLS);

            for(std::uint64_t i = 0; i < rows.size(); ++i) {
                assert(rows[i].id == i + 1);
            }

            auto first = table.find(1);
            assert(first.has_value());
            assert(first->id == 1);

            auto middle = table.find((LEAF_NODE_MAX_CELLS + 1) / 2);
            assert(middle.has_value());

            auto last = table.find((LEAF_NODE_MAX_CELLS));
            assert(last.has_value());
            assert(last->id == LEAF_NODE_MAX_CELLS);

            auto missing = table.find(9999);
            assert(!missing.has_value());
        }

        {
            Table table(split_file_name);

            auto rows = table.select_all();
            assert(rows.size() == LEAF_NODE_MAX_CELLS);

            for (std::uint64_t i = 0; i < rows.size(); ++i) {
                assert(rows[i].id == i + 1);
            }

            auto first = table.find(1);
            assert(first.has_value());

            auto last = table.find(LEAF_NODE_MAX_CELLS);
            assert(last.has_value());

            auto missing = table.find(9999);
            assert(!missing.has_value());
        }

        std::remove(split_file_name.c_str());
    }
    
    return 0;
};