cmake_minimum_required(VERSION 3.7.2)

project(orderbook)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(orderbook main.cpp orderbook.cpp)  
