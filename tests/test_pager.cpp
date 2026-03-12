#include "pager.hpp"

#include <cassert>
#include <cstdio>

int main () {
    const std::string filename = "test.db";

    {
        Pager pager(filename);
        auto& page = pager.get_page(0);
        page[0] = 'A';
        page[1] = 'B';
        page[2] = 'C';
        pager.flush_page(0);
    }

    {
        Pager pager(filename);
        auto& page = pager.get_page(0);
        assert(page[0] == 'A');
        assert(page[1] == 'B');
        assert(page[2] == 'C');
    }

    std::remove(filename.c_str());
    return 0;
}