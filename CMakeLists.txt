cmake_minimum_required(VERSION 3.20)
project(mini_db VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(mini_db
    src/pager.cpp
    src/row.cpp
    src/table.cpp
    src/btree.cpp
)

target_include_directories(mini_db PUBLIC include)

add_executable(mini_db_cli src/main.cpp)
target_link_libraries(mini_db_cli PRIVATE mini_db)

enable_testing()

add_executable(test_row tests/test_row.cpp)
target_link_libraries(test_row PRIVATE mini_db)

add_test(NAME row_test COMMAND test_row)

add_executable(test_pager tests/test_pager.cpp)
target_link_libraries(test_pager PRIVATE mini_db)
add_test(NAME pager_test COMMAND test_pager)

add_executable(test_table tests/test_table.cpp)
target_link_libraries(test_table PRIVATE mini_db)
add_test(NAME table_test COMMAND test_table)