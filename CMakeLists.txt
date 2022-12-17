cmake_minimum_required(VERSION 3.23)
project(QUEEN_GA_WITH_MPI)

set(CMAKE_CXX_STANDARD 14)

# Add MPI Package to Project
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
find_package(MPI REQUIRED)

add_executable(QUEEN_GA_WITH_MPI main.cpp)

# Add libraries for code completion and compiling
target_link_libraries(QUEEN_GA_WITH_MPI PUBLIC MPI::MPI_CXX)