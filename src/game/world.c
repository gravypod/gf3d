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

world_chunk_t *world_chunk_get(chunk_location *cl)
{
    for (size_t i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i]->location.x == cl->x && world.chunks[i]->location.z == cl->z)
            return world.chunks[i];
    }
    return NULL;
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

bool world_collision_find(const vec3 center, float x_radius, float y_radius, float z_radius, location *blocking_block)
{
    vec3 back_bottom_left = {center[0] - x_radius, center[1] - y_radius, center[2] - z_radius };
    vec3 top_front_right  = {center[0] + x_radius, center[1] + y_radius, center[2] + z_radius };

    for (long x = (long) back_bottom_left[0]; x < ceilf(top_front_right[0]); x++) {
        for (long y = (long) back_bottom_left[1]; y < ceilf(top_front_right[1]); y++) {
            for (long z = (long) back_bottom_left[2]; z < ceilf(top_front_right[2]); z++) {
                location l = {
                        x, y, z
                };
                chunk_location cl;
                location_to_chunk_location(&l, &cl);

                world_chunk_t *chunk = world_chunk_get(&cl);

                if (chunk) {
                    if (world_chunk_location_exists(chunk, &l)) {

                        // Send block position to caller
                        if (blocking_block != NULL) {
                            memcpy(blocking_block, &l, sizeof(l));
                        }

                        return true;
                    }

                }
            }
        }
    }

    return false;
}
