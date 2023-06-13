cmake_minimum_required(VERSION 2.8.8)

project (include)

#cJSON include
set (CJSON_SOURCE_FILE "cJSON.c")
set (CJSON_HEADER_FILE "cJSON.h")

#printer_state include
set (PRINTER_STATE_HEADER_FILE "printer_state.h")

add_library (cJSON STATIC ${CJSON_HEADER_FILE} ${CJSON_SOURCE_FILE})

