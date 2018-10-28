cmake_minimum_required(VERSION 3.5)
project(gf3d)

# Requirements
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

set(ENGINE_LIBRARIES m vulkan ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${CMAKE_SOURCE_DIR}/libs/libsj.a)

set(GAME_SOURCE_FILES
        src/game/game.c
        src/game/entity/entity.c src/game/entity/entity.h
        src/game/entity/manager.c src/game/entity/manager.h
        src/game/entity/definitions/agumon.c src/game/entity/definitions/agumon.h)

# ALL SOURCES
set(SOURCE_FILES
        src/gf3d_camera.c
        src/gf3d_commands.c
        src/gf3d_extensions.c
        src/gf3d_mesh.c
        src/gf3d_model.c
        src/gf3d_obj_load.c
        src/gf3d_pipeline.c
        src/gf3d_shaders.c
        src/gf3d_swapchain.c
        src/gf3d_texture.c
        src/gf3d_types.c
        src/gf3d_validation.c
        src/gf3d_vgraphics.c
        include/gf3d_uniforms.h src/gf3d_uniforms.c
        src/gf3d_vqueues.c
        src/simple_logger.c)

include_directories(include libs/include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${GAME_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${ENGINE_LIBRARIES})

