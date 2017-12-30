cmake_minimum_required(VERSION 3.8)
project(ludum-dare-40)

find_package(glfw3 REQUIRED)
find_package(OpenAL REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(game/src/libs/raylib)

add_executable(ludum-dare-40 "")
target_link_libraries(ludum-dare-40 PRIVATE raylib)
target_include_directories(ludum-dare-40 PUBLIC game/src/include)
target_sources(ludum-dare-40 PRIVATE
        # sources
        game/src/collision.c
        game/src/levelgen.c
        game/src/main.c

        # headers
        game/src/entity.h
        game/src/hashset.h
        game/src/item.h
        game/src/main.h
        game/src/vector2i.h)