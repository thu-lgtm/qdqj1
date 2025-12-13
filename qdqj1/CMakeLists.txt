cmake_minimum_required(VERSION 4.0)
project(qdqj1)

set(CMAKE_CXX_STANDARD 17)

add_executable(qdqj1 main.cpp
        Strategy.h
        Strategy.cpp
        Game.h
        Enums.h
        Structs.h
        Player.h
        Player.cpp
        Game.cpp)
