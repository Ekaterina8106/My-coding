cmake_minimum_required(VERSION 3.10)
project(linear_systems)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Создаем исполняемый файл из указанных исходников
add_executable(a.out
    src/main.cpp
    src/matrix.cpp
    src/inverse_gauss.cpp
)
