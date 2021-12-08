cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME "curl_client")
set (PROJECT_NAME_TESTS "curl_client_tests")

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

find_package (CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
message(STATUS "CURL INCLUDE DIRS ${CURL_INCLUDE_DIRS}")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
message(STATUS "GTEST INCLUDE DIRS ${GTEST_INCLUDE_DIRS}")

include_directories(include)


add_library(${PROJECT_NAME}
        src/data_structures.cpp
        src/curl_handler.cpp
        src/request.cpp
        src/json.cpp
        src/utils.cpp
        src/connection_data.cpp
        )
target_link_libraries(${PROJECT_NAME}
        ${CURL_LIBRARY}
        )

add_executable(${PROJECT_NAME_TESTS} tests/request_tests.cpp)
target_link_libraries(${PROJECT_NAME_TESTS}
        ${GTEST_LIBRARIES}
        pthread
        )
