cmake_minimum_required(VERSION 2.8.8)

project (test)

find_package(Cups REQUIRED)

add_executable(test_attr "test_attr.c")
add_executable(test_dest "print.c")

target_include_directories(test_attr PRIVATE ${CUPS_INCLUDE_DIR})
target_link_libraries(test_attr PRIVATE ${CUPS_LIBRARIES})

target_include_directories(test_dest PRIVATE ${CUPS_INCLUDE_DIR})
target_link_libraries(test_dest PRIVATE ${CUPS_LIBRARIES})




