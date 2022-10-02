cmake_minimum_required(VERSION 3.23)
project(hashtable C)

set(CMAKE_C_STANDARD 99)

add_executable(hashtable main.c src/hash_table.c)
