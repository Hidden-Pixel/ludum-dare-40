project(ludum-dare-40)


add_executable(ludum-dare-40 "")
target_link_libraries(ludum-dare-40 PRIVATE glfw )
target_include_directories(ludum-dare-40 PRIVATE game/src/include)
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