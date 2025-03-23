cmake_minimum_required(VERSION 3.11.0)
project(tetris VERSION 0.0.1)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include(cmake/CPM.cmake)

CPMAddPackage(
	NAME raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG 5.0
)

add_executable(tetris
	src/main.cpp)

target_link_libraries(tetris PRIVATE raylib)
set_property(TARGET tetris PROPERTY CXX_STANDARD 20)
