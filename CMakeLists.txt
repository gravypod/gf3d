cmake_minimum_required(VERSION 3.5)
project(gf3d)

# Requirements
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

#glslangValidator -V -o voxel.vert.spv voxel.vert

file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/shaders/*.frag"
        "${CMAKE_SOURCE_DIR}/shaders/*.geom"
        "${CMAKE_SOURCE_DIR}/shaders/*.vert"
        )
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/shaders/"
            COMMAND glslangValidator -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)
add_custom_target(
        COMPILE_SHADERS
        DEPENDS ${SPIRV_BINARY_FILES}
)


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
        src/simple_logger.c src/game/entity/definitions/player.c src/game/entity/definitions/player.h include/gf3d_collide.h src/gf3d_collide.c src/game/world.c src/game/world.h src/game/noise.c src/game/noise.h src/game/coords.h src/game/shapes/box.c src/game/shapes/box.h src/game/coords.c src/game/structures/mdarray.h src/game/chunk.c src/game/chunk.h src/game/rendering/world.c src/game/rendering/world.h)

include_directories(include libs/include ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${GAME_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${ENGINE_LIBRARIES})
add_dependencies(${PROJECT_NAME} COMPILE_SHADERS)

