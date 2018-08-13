cmake_minimum_required(VERSION 3.10)
project(mpi_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER /usr/bin/mpicc)
set(CMAKE_C_FLAGS "-O2")

include_directories(/usr/include/mpi/)

add_executable(mpi_test convolute.c convolute.h pixel.h filters.h image.h utils.c utils.h)
add_executable(mpi_arg_test main.c args.h utils.c utils.h neighbours.c neighbours.h convolute.c convolute.h directions.h args.c io.c io.h work.c work.h)

target_link_libraries(mpi_arg_test m)