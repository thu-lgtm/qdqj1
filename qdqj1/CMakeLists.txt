cmake_minimum_required(VERSION 4.0)
project(try_1)

set(CMAKE_CXX_STANDARD 20)

add_executable(try_1 main.cpp
        Enums.cpp
        Enums.h
        Structs.h
        Player.cpp
        Player.h
        Game.cpp
        Game.h
        Strategy.cpp  # 新增
        Strategy.h
        Enums.cpp
        Extension.h
        CardDatabase.h
        CardDatabase.cpp    # 新增
)
