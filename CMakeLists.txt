cmake_minimum_required(VERSION 3.17)
project(MpiBench C)

find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

set(CMAKE_C_STANDARD 99)

add_executable(MpiBench point_comm.c)
add_executable(CollectiveBench collective_comm.c)
add_executable(MPIIO mpiio.c)

target_link_libraries(MpiBench ${MPI_C_LIBRARIES})
target_link_libraries(CollectiveBench ${MPI_C_LIBRARIES})
target_link_libraries(MPIIO ${MPI_C_LIBRARIES})