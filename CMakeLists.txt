cmake_minimum_required(VERSION 3.16.3)
project(ble_sensor_cgi)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.71.0 COMPONENTS filesystem REQUIRED)
find_package(CURL REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
        http_parser.hpp http_parser.cpp
        hex_dump.hpp hex_dump.cpp
        http_io.cpp http_io.hpp
        string_util.cpp string_util.hpp
        magic_mime.hpp magic_mime.cpp
        base64.hpp base64.cpp
        jsonxx/jsonxx.h jsonxx/jsonxx.cc
        debug_output.hpp
        ble_bme_680_sqlite3.hpp ble_bme_680_sqlite3.cpp
        main.cpp)

include(libmagic.cmake)

target_include_directories(${PROJECT_NAME} BEFORE PUBLIC ${CURL_INCLUDE_DIR}/curl)

target_link_libraries(${PROJECT_NAME} curl ${Boost_LIBRARIES})
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-lsqlite3 -llzma -lbz2)
