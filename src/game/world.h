#ifndef GF3D_WORLD_H
#define GF3D_WORLD_H

#include <stdbool.h>
#include <stdlib.h>
#include "coords.h"
#include "entity/entity.h"
#include "chunk.h"

#define MAX_CHUNKS_PLAYER_DISTANCE 3
#define MAX_NUM_LOADED_CHUNKS (MAX_CHUNKS_PLAYER_DISTANCE * MAX_CHUNKS_PLAYER_DISTANCE)

typedef struct
{
    long seed;
    uint32_t num_blocks;
    uint32_t num_chunks;
    rendering_pipeline_world *rendering;

    world_chunk_t *chunks[MAX_NUM_LOADED_CHUNKS];
} world_t;


void world_init();
void world_update();
void world_render(VkCommandBuffer buffer, Uint32 frame);
bool world_collision_find(const vec3 center, float x_radius, float y_radius, float z_radius, location *blocking_block);

#endif
