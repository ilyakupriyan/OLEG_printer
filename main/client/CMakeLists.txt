cmake_minimum_required(VERSION 2.8.8)

project (client)

add_executable(client "printer_client.cpp")

target_include_directories(client PRIVATE ${CUPS_INCLUDE_DIR})
target_link_libraries(client PRIVATE ${CUPS_LIBRARIES})