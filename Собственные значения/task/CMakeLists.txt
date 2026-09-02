cmake_minimum_required(VERSION 3.10)
project(eigen_values)

set(CMAKE_CXX_STANDARD 11)

add_executable(a.out
    src/main.cpp
    src/matrix.cpp
    src/eigenvalues.cpp
)
