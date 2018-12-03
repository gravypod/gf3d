#include "bprune.h"


long bprune_xz_flat_coord(long x, long z)
{
    return x + (z * SIZE_CHUNK_X);
}

int *bprune_chunk_count_columns(const world_chunk_t *chunk)
{
    static int counts[SIZE_CHUNK_X * SIZE_CHUNK_Z];

    block_location bl;
    for (bl.x = 0; bl.x < SIZE_CHUNK_X; bl.x++) {
        for (bl.z = 0; bl.z < SIZE_CHUNK_Z; bl.z++) {
            long i = bprune_xz_flat_coord(bl.x, bl.z);
            // Reset counter
            counts[i] = 0;

            for (bl.y = 0; bl.y < SIZE_CHUNK_Y; bl.y++) {
                if (world_chunk_block_location_exists(chunk, &bl)) {
                    counts[i] += 1;
                }
            }
        }
    }

    return counts;
}

bool bprune_chunk_keep_block(const world_chunk_t *chunk, const block_location *bl)
{
    if (!world_chunk_block_location_exists(chunk, bl)) {
        return false;
    }

    // Test box outsides
    if (bl->x == 0 || bl->x + 1 == SIZE_CHUNK_X) {
        return true;
    }

    if (bl->y == 0 || bl->y + 1 == SIZE_CHUNK_Y) {
        return true;
    }

    if (bl->z == 0 || bl->z + 1 == SIZE_CHUNK_Z) {
        return true;
    }

    const block_location surroundings[6] = {
            {bl->x + 1, bl->y,     bl->z},
            {bl->x - 1, bl->y,     bl->z},
            {bl->x,     bl->y + 1, bl->z},
            {bl->x,     bl->y - 1, bl->z},
            {bl->x,     bl->y,     bl->z + 1},
            {bl->x,     bl->y,     bl->z - 1},
    };

    for (int i = 0; i < 6; i++) {
        if (!world_chunk_block_location_exists(chunk, &surroundings[i])) {
            return true;
        }
    }

    return false;
}

block_location *bprune_chunk(const world_chunk_t *chunk, size_t *their_blocks)
{
    static block_location keep[SIZE_CHUNK_BLOCKS];
    const int *counts = bprune_chunk_count_columns(chunk);


    size_t blocks = 0;
    block_location bl;
    for (bl.x = 0; bl.x < SIZE_CHUNK_X; bl.x++) {
        for (bl.z = 0; bl.z < SIZE_CHUNK_Z; bl.z++) {
            int remaining_blocks_in_column = counts[bprune_xz_flat_coord(bl.x, bl.z)];

            for (bl.y = 0; (bl.y < SIZE_CHUNK_Y) && remaining_blocks_in_column > 0; bl.y++) {
                if (bprune_chunk_keep_block(chunk, &bl)) {
                    keep[blocks] = bl;
                }
                remaining_blocks_in_column -= 1;
                blocks += 1;
            }
        }
    }

    *their_blocks = blocks;
    return keep;
}