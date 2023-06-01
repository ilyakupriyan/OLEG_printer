CMAKE_MINIMUM_REQUIRED_VERSION (VERSION 2.8.8)

project (server)

add_executable(server "printer_server.c")

target_include_directories(server PRIVATE ${CUPS_INCLUDE_DIR})
target_link_libraries(server PRIVATE ${CUPS_LIBRARIES})