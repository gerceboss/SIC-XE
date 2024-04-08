cmake_minimum_required(VERSION 3.10)
project(SIC-XE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define your header files directory
include_directories(include)

# Define your source files directory
file(GLOB SOURCES "src/*.cpp")

# Create executable
add_executable(assembler ${SOURCES})
