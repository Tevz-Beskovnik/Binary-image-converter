cmake_minimum_required(VERSION 3.27)
project(bic)

set(CMAKE_CXX_STANDARD 11)

add_executable(bic main.cpp)

add_definitions(-DVERSION="0.1.0")

target_include_directories(
        "${CMAKE_PROJECT_NAME}"
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
)
