cmake_minimum_required(VERSION 3.20.0-rc1)

project(IAM962)

# Set variables

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add externals

add_subdirectory(external/linalg)

# Add projects

add_subdirectory(project1a)
add_subdirectory(project1b)
add_subdirectory(project1c)

add_subdirectory(project2a)