cmake_minimum_required (VERSION 2.8.8)

project (client)

add_executable(client "printer_client.cpp")

include_directories("$(PROJECT_SOURCE_DIR)/../include")

#cJSON include
target_link_libraries(client cJSON)