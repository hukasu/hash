cmake_minimum_required(VERSION 3.12)

project("hash")

option(ENABLE_TESTING "Enable testing?" OFF)

add_library(
    md5
    src/md5.cpp
)

add_executable(
    hash
    src/main.cpp 
)
target_link_libraries(
    hash
    md5
)

if (ENABLE_TESTING)
    enable_testing()
    include(CTest)
    include(tests/tests.cmake)
endif(ENABLE_TESTING)