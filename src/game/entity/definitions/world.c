#include "world.h"
#include <game/rendering/world.h>
#include <game/chunk.h>

entity_t *world_entity;

#define SIZE_WORLD_BLOCKS (MAX_NUM_LOADED_CHUNKS * SIZE_CHUNK_BLOCKS)


typedef struct
{
    long seed;
    uint32_t num_blocks;
    uint32_t num_chunks;
    rendering_pipeline_world *rendering;

    world_chunk_t *chunks[MAX_NUM_LOADED_CHUNKS];
} world_t;


world_t world;


world_chunk_t *world_chunk_get(chunk_location *cl)
{
    for (size_t i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i]->location.x == cl->x && world.chunks[i]->location.z == cl->z)
            return world.chunks[i];
    }
    return NULL;
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


void entity_world_update(entity_t *entity, void *metadata)
{
    static gpublock temp_block_buffer[SIZE_WORLD_BLOCKS];
    world.num_blocks = 0;

    for (int i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i]) {
            world_chunk_update(world.chunks[i]);
            world.num_blocks += world_chunk_gpu_send(world.chunks[i], temp_block_buffer + world.num_blocks);
        }
    }

    rendering_pipeline_world_block_data_submit(world.rendering, temp_block_buffer, world.num_blocks);
}

void entity_world_render(entity_t *entity, const entity_render_pass_t *render_pass)
{
    rendering_pipeline_world_renderpass(world.rendering, world.num_blocks, render_pass->commandBuffer, render_pass->bufferFrame);
}

void entity_world_init(entity_t *entity, void *metadata)
{
    // Track entity pointer
    world_entity = entity;

    // Initialize entity structure
    entity->update = entity_world_update;
    entity->draw = entity_world_render;


    world.seed = 10;
    world.rendering = rendering_pipeline_world_init(SIZE_WORLD_BLOCKS);

    // Generate the first chunks
    for (long x = 0; x < MAX_NUM_LOADED_CHUNKS; x++) {
        const chunk_location cl = {
                x, 0
        };
        world.chunks[x] = world_chunk_load(world.seed, cl);
    }
}
