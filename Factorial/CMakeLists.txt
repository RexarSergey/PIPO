cmake_minimum_required(VERSION 3.24)
project(PIPO C)

set(CMAKE_C_STANDARD 17)

add_executable(Factorial factorial_process.c factorial_thread.c)
