#include <malloc.h>
#include "chunk.h"
#include "noise.h"
#include "structures/bprune.h"

world_chunk_t *world_chunk_allocate(const chunk_location cl)
{
    world_chunk_t *chunk = malloc(sizeof(world_chunk_t));
    chunk->location = cl;
    return chunk;
}

void world_chunk_generate(long seed, world_chunk_t *chunk)
{
    memset(chunk->blocks, 0, sizeof(char) * SIZE_CHUNK_BLOCKS);

    block_location bl;
    for (bl.x = 0; bl.x < SIZE_CHUNK_X; bl.x++) {
        for (bl.y = 0; bl.y < SIZE_CHUNK_Y; bl.y++) {
            for (bl.z = 0; bl.z < SIZE_CHUNK_Z; bl.z++) {
                bool block_should_be_placed = noise_block_exists(seed, bl.x, bl.y, bl.z);

                if (!block_should_be_placed && !(bl.x == 0 && bl.y == 0 && bl.z == 0)) {
                    continue;
                }

                long location = block_location_to_index(&bl);
                chunk->blocks[location] = 1;
            }
        }
    }
}

world_chunk_t *world_chunk_load(long seed, const chunk_location cl)
{
    // TODO: Persist to disk
    world_chunk_t *chunk = world_chunk_allocate(cl);
    world_chunk_generate(seed, chunk);
    return chunk;
}

bool world_chunk_block_location_exists(const world_chunk_t *chunk, const block_location *bl)
{
    if (bl->x >= SIZE_CHUNK_X || bl->y >= SIZE_CHUNK_Y || bl->z >= SIZE_CHUNK_Z || bl->z < 0.0f) {
        return false;
    }
    long location = block_location_to_index(bl);
    return chunk->blocks[location] != 0;
}

bool world_chunk_location_exists(const world_chunk_t *chunk, location *l)
{
    block_location bl;
    location_to_block_location(l, &bl);
    return world_chunk_block_location_exists(chunk, &bl);
}


size_t world_chunk_gpu_serialize(const world_chunk_t *chunk, gpublock *blocks)
{
    size_t num_blocks = 0;

    block_location *keep = bprune_chunk(chunk, &num_blocks);

    for (size_t i = 0; i < num_blocks; i++) {
        block_location *bl = &keep[i];
        location l;

        // convert local location to world space location
        location_from_chunk_block(&chunk->location, bl, &l);

        // Serialize block to GPU vector
        gpublock *block = &blocks[i];

        block->position[0] = (float) l.x;
        block->position[1] = (float) l.y;
        block->position[2] = (float) l.z;
    }

    return num_blocks;
}