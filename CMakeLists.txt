cmake_minimum_required(VERSION 3.10)
project(parsys C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER /usr/bin/mpicc)

include_directories(/usr/include/mpi/)

add_executable(run_conv main.c)