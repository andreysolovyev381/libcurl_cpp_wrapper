set(LIBRARY_NAME curl_client)

find_package(CURL REQUIRED)
if(CURL_FOUND)
    message(STATUS "CURL FOUND: including and linking...")
    include_directories(${CURL_INCLUDE_DIRS})
else()
    message(STATUS "NOT FOUND CURL")
endif()

add_library(${LIBRARY_NAME}
        ./src/json.cpp
        ./src/curl_handler.cpp
        ./src/url.cpp
        ./src/request.cpp
)
target_compile_options(${LIBRARY_NAME}
        PRIVATE
)
target_include_directories(${LIBRARY_NAME}
        PUBLIC
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR}/src/executable
)
target_link_libraries(${LIBRARY_NAME}
        PRIVATE
        ${CURL_LIBRARY}
)

set_target_properties(
        ${LIBRARY_NAME}
        PROPERTIES
        OUTPUT_NAME ${LIBRARY_NAME}
        SUFFIX ".a"
)