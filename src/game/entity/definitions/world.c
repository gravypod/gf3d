#include "world.h"
#include "player.h"
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

long world_chunk_idx(const chunk_location *cl)
{

    for (size_t i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i] != NULL && world.chunks[i]->location.x == cl->x && world.chunks[i]->location.z == cl->z) {
            return i;
        }
    }
    return -1;
}

world_chunk_t *world_chunk_get(const chunk_location *cl)
{
    long i = world_chunk_idx(cl);

    if (i == -1) {
        return NULL;
    }

    return world.chunks[i];
}

void world_make_sure_loaded(const chunk_location *cl)
{
    if (world_chunk_get(cl)) {
        return;
    }

    for (size_t i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i] == NULL) {
            world.chunks[i] = world_chunk_load(world.seed, cl);
            return;
        }
    }
}

void world_make_sure_unloaded(const chunk_location *cl)
{
    long idx = world_chunk_idx(cl);
    if (idx == -1)
        return;

    long last_idx = -1;
    for (size_t i = (size_t) idx; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i] != NULL) {
            last_idx = i;
        }
    }

    world_chunk_free(world.chunks[idx]);
    if (last_idx == -1) {
        world.chunks[idx] = NULL;
    } else {
        world.chunks[idx] = world.chunks[last_idx];
        world.chunks[last_idx] = NULL;
    }

}


long world_height(location *l)
{
    chunk_location cl;
    location_to_chunk_location(l, &cl);

    world_chunk_t *c = world_chunk_get(&cl);

    if (!c) {
        return -1;
    }

    return world_chunk_height(c, l);
}


bool chunk_in_list(size_t num_list, chunk_location *required_chunk_locations, chunk_location *cl)
{
    for (size_t i = 0; i < num_list; i++) {
        if (required_chunk_locations[i].x == cl->x && required_chunk_locations[i].z == cl->z) {
            return true;
        }
    }
    return false;
}

void world_unload_uneeded_chunks(size_t num_chunks, chunk_location *required_chunk_locations)
{
    static chunk_location freeable_locations[MAX_NUM_LOADED_CHUNKS];

    size_t num_to_free = num_chunks;

    for (size_t i = 0; i < num_chunks; i++) {
        if (world_chunk_get(&required_chunk_locations[i])) {
            num_to_free--;
        }
    }

    size_t freeidx = 0;
    for (size_t i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world.chunks[i]) {
            if (!chunk_in_list(num_chunks, required_chunk_locations, &world.chunks[i]->location)) {
                num_to_free--;
                freeable_locations[freeidx++] = world.chunks[i]->location;
            }
        }
    }

    for (size_t i = 0; i < freeidx; i++) {
        world_make_sure_unloaded(&freeable_locations[i]);
    }
}

void entity_world_load_nearby_chunks(const chunk_location *center_chunk)
{
    static chunk_location required_chunk_locations[MAX_NUM_LOADED_CHUNKS];
    size_t num_chunks = 0;

    for (long x = -3; x <= 3; x++) {
        for (long z = -3; z <= 3; z++) {
            chunk_location *cl = &required_chunk_locations[num_chunks++];
            cl->x = center_chunk->x + x;
            cl->z= center_chunk->z + z;
        }
    }

    world_unload_uneeded_chunks(num_chunks, required_chunk_locations);

    for (size_t i = 0; i < num_chunks; i++) {
        world_make_sure_loaded(&required_chunk_locations[i]);
    }
}

void entity_world_update(entity_t *entity, void *metadata)
{
    static gpublock temp_block_buffer[SIZE_WORLD_BLOCKS];
    static chunk_location center;

    location player_location = {
            .x = (long) player_entity->position[0],
            .y = (long) player_entity->position[1],
            .z = (long) player_entity->position[2],
    };

    location_to_chunk_location(&player_location, &center);
    entity_world_load_nearby_chunks(&center);

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


    world.seed = 4353453;
    world.rendering = rendering_pipeline_world_init(SIZE_WORLD_BLOCKS);
}
