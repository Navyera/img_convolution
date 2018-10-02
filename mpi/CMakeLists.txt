cmake_minimum_required(VERSION 3.10)
project(mpi_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER /usr/bin/mpicc)
set(CMAKE_C_FLAGS "-O2 -D FILTER=identity")

include_directories(/usr/include/mpi/)
include_directories(include)

add_executable(mpi_test src/args.c src/convolute.c src/io.c src/main.c src/neighbours.c src/utils.c src/work.c)

target_link_libraries(mpi_test m)