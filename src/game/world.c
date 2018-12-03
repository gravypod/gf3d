#include <gf3d_pipeline.h>
#include <gf3d_vgraphics.h>
#include <simple_logger.h>
#include <gf3d_swapchain.h>
#include "world.h"
#include "noise.h"
#include "chunk.h"
#include "entity/definitions/player.h"
#include "shapes/box.h"

#define SIZE_WORLD_BLOCKS (MAX_NUM_LOADED_CHUNKS * SIZE_CHUNK_BLOCKS)

world_t world = {
        .seed = 10
};


void world_update()
{
    static gpublock temp_block_buffer[SIZE_WORLD_BLOCKS];
    world.num_blocks = 0;

    for (int i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i]) {
            world.num_blocks += world_chunk_gpu_serialize(world.chunks[i], temp_block_buffer + world.num_blocks);
        }
    }

    rendering_pipeline_world_block_data_submit(world.rendering, temp_block_buffer, world.num_blocks);
}

void world_render(VkCommandBuffer buffer, Uint32 frame)
{
    rendering_pipeline_world_renderpass(world.rendering, world.num_blocks, buffer, frame);
}

void world_chunks_init()
{
    for (long x = 0; x < MAX_NUM_LOADED_CHUNKS; x++) {
        const chunk_location cl = {
                x, 0
        };
        world.chunks[x] = world_chunk_load(world.seed, cl);
    }
}

void world_init()
{
    world.rendering = rendering_pipeline_world_init(SIZE_WORLD_BLOCKS);
    world_chunks_init();
}
/*

world_collision_result world_collision_check(vec3 center, float size_x, float size_y, float size_z)
{
    const box3d left = {
            .bottom_back_left = {
                    center[0] - size_x, center[1] - size_y, center[2] - size_z
            },
            .x_size = size_x * 2,
            .y_size = size_y * 2,
            .z_size = size_z * 2,
    };
    box3d right = {
            .bottom_back_left = {
                    0.0f, 0.0f, 0.0f
            },
            .x_size = 1.0f,
            .y_size = 1.0f,
            .z_size = 1.0f,
    };

    for (long x = (long) floorf(left.bottom_back_left.x); x < (long) ceilf(left.bottom_back_left.x + left.x_size); x++) {
        for (long y = (long) floorf(left.bottom_back_left.y); y < (long) ceilf(left.bottom_back_left.y + left.y_size); y++) {
            for (long z = (long) floorf(left.bottom_back_left.z); z < (long) ceilf(left.bottom_back_left.z + left.z_size) && z > 0.0f; z++) {


                right.bottom_back_left.x = x;
                right.bottom_back_left.y = y;
                right.bottom_back_left.z = z;

                if (box3d_colliding(&left, &right)) {
                    const chunk_t *c = world_chunk_get(&world, x, y);

                    if (c) {

                        const long block_idx = world_chunk_blocks_index(x, y, z);

                        if (c->blocks[block_idx]) {
                            return COLLISION;
                        }
                    }
                }
            }
        }
    }

    return NO_COLLISSION;
}*/
