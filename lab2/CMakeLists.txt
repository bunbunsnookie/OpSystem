cmake_minimum_required(VERSION 3.15)
project(OpSystem2)

set(APP_SRCS main.cpp)
set(LIB_SRCS background_mode.cpp)
set(LIB_HDRS background_mode.hpp)

add_library(background_mode SHARED ${LIB_HDRS} ${LIB_SRCS})
add_executable(OpSystem2 main.cpp ${LIB_HDRS} ${LIB_SRCS})

target_link_libraries(OpSystem2 background_mode)