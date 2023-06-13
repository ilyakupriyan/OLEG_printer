cmake_minimum_required (VERSION 2.8.8)

project (server)

add_executable(server "printer_server.c")

include_directories("$(PROJECT_SOURCE_DIR)/../include")

#cJSON include
target_link_libraries(server cJSON)

#CUPS include
target_include_directories(server PRIVATE ${CUPS_INCLUDE_DIR})
target_link_libraries(server PRIVATE ${CUPS_LIBRARIES})