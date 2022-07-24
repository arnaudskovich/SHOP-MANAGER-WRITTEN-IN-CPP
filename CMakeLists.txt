cmake_minimum_required(VERSION 3.22)
project(INF200)

set(CMAKE_CXX_STANDARD 14)

add_executable(INF200 main.cpp sell.cpp stocks.cpp sell.h stocks.h shops.cpp shops.h product.h global.h utils.cpp product.cpp)
