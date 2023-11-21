cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)

project(main LANGUAGES CXX)

find_package(spdlog REQUIRED)
find_package(magic_enum REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE magic_enum::magic_enum)
target_link_libraries(main PRIVATE spdlog::spdlog)
