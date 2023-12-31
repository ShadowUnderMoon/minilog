cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

project(minilog LANGUAGES CXX)

# find_package(spdlog REQUIRED)
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "use std::format" FORCE)
add_subdirectory(spdlog)
find_package(magic_enum REQUIRED)
# find_package(fmt REQUIRED)

file(GLOB SOURCES "src/*.cpp")
# add_compile_definitions(SPDLOG_USE_STD_FORMAT)
add_executable(main ${SOURCES})
target_include_directories(main PUBLIC include)
target_link_libraries(main PRIVATE magic_enum::magic_enum)
# target_link_libraries(main PRIVATE spdlog::spdlog)
target_link_libraries(main PRIVATE spdlog::spdlog)
# target_link_libraries(main PRIVATE fmt::fmt)
target_include_directories(main PUBLIC /usr/include/mysql)
target_include_directories(main PUBLIC /usr/include/mysql++)
target_link_libraries(main PUBLIC mysqlpp)
