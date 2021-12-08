cmake_minimum_required(VERSION 3.19)
set (LIB_USAGE "lib_usage")

project(${LIB_USAGE})

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(lib)  #allows to select target

find_package (CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
message(STATUS "CURL INCLUDE DIRS ${CURL_INCLUDE_DIRS}")

add_executable(${LIB_USAGE} main.cpp)
target_link_libraries(${LIB_USAGE}
        ${CURL_LIBRARY}
        /Users/elenasolovyeva/Documents/Cpp/Projects/curl_client/cmake-build-debug/lib/libcurl_client.a
        )
