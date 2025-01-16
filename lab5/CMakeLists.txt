cmake_minimum_required(VERSION 3.15)
project(OpSystem5)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

ADD_EXECUTABLE(main server.hpp main.cpp)
IF (WIN32)
    TARGET_LINK_LIBRARIES(main ws2_32)
ENDIF(WIN32)