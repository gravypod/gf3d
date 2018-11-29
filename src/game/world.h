#ifndef GF3D_WORLD_H
#define GF3D_WORLD_H

#include <stdbool.h>
#include <stdlib.h>
#include "coords.h"
#include "entity/entity.h"

#define CHUNK_X 16
#define CHUNK_Y 16
#define CHUNK_Z 16

#define CHUNK_SIZE (CHUNK_X * CHUNK_Y * CHUNK_Z)

// Convert a chunk x to a world x
#define CHUNK_TO_WORLD_X(x) (x * CHUNK_X)
#define CHUNK_TO_WORLD_Y(y) (y * CHUNK_Y)

#define WORLD_TO_CHUNK_X(x) (x / CHUNK_X)
#define WORLD_TO_CHUNK_Y(y) (y / CHUNK_Y)

#define MAX_CHUNK_DISTANCE 3
#define MAX_NUM_LOADED_CHUNKS (MAX_CHUNK_DISTANCE * MAX_CHUNK_DISTANCE)

#define MAX_NUM_LOADED_BLOCKS (MAX_NUM_LOADED_CHUNKS * CHUNK_SIZE)

typedef struct
{
    long major_x, major_y;
    char blocks[CHUNK_SIZE];
} chunk_t;

typedef struct
{
    long seed;
    chunk_t chunks[MAX_NUM_LOADED_CHUNKS];
} world_t;

typedef struct
{
    long *x, *y, *z;
    char *type;
} world_block_view_t;

/**
 * Find the index of a chunk within the block array of the chunk
 * @param x - World x block position
 * @param y - World y block position
 * @param z - World z block position
 * @return
 */
static inline long world_chunk_blocks_index(long x, long y, long z)
{
    const long offset_x = labs(x % CHUNK_X);
    const long offset_y = labs(y % CHUNK_Y);
    const long offset_z = labs(z % CHUNK_Z);

    return coord_flatten_3d_to_1d(offset_x, offset_y, offset_z, CHUNK_X, CHUNK_Y);
}

static inline void world_chunk_world_index(long block_idx, long *x, long *y, long *z)
{
    coord_expand_1d_to_3d(block_idx, CHUNK_X, CHUNK_Y, x, y, z);
}

/**
 * Check to see if the chunk is loaded
 * @param w - World
 * @param chunk_x - X chunk location
 * @param chunk_y - Y chunk location
 * @return
 */
static inline bool world_chunk_is_loaded(world_t *w, long chunk_x, long chunk_y)
{
    for (int i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (w->chunks[i].major_x == chunk_x || w->chunks[i].major_y == chunk_y) {
            return true;
        }
    }

    return false;
}


/**
 * Check to see if a block could exist within the pool of existing chunks
 * @param w - World
 * @param x - X location of the block
 * @param y - Y location of the block
 * @return
 */
static inline bool world_chunk_blocks_is_loaded(world_t *w, long x, long y)
{
    const long chunk_x = x / CHUNK_X;
    const long chunk_y = y / CHUNK_Y;

    for (int i = 0; i < MAX_NUM_LOADED_CHUNKS; i++) {
        if (world_chunk_is_loaded(w, chunk_x, chunk_y)) {
            return true;
        }
    }

    return false;
}

void world_init();
void world_update();
void world_render(VkCommandBuffer buffer, Uint32 frame);

#endif
