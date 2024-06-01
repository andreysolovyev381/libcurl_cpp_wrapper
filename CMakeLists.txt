cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME "Curl client mini library")

project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "C++20 wrapper for libcurl with a bare minimum of functionality"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(curl_client)
add_subdirectory(./tests)
