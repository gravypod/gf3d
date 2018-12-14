#ifndef GF3D_CHUNK_H
#define GF3D_CHUNK_H

#include <stddef.h>
#include <linmath.h>
#include <stdbool.h>
#include "coords.h"
#include "rendering/world.h"

#define SIZE_CHUNK_BLOCKS  ((SIZE_CHUNK_X) * (SIZE_CHUNK_Y) * (SIZE_CHUNK_Z))

/**
 * Index metadata for a chunk used for housekeeping.
 */
typedef struct
{
    /**
     * Num blocks inside of this chunk that exist
     */
    size_t num_blocks;
    /**
     * Count of blocks in each column. This is used to prune search of a column.
     */
    int num_blocks_in_columns[SIZE_CHUNK_X * SIZE_CHUNK_Z];
} world_chunk_index_t;

/**
 * Cache of data used for rendering a chunk
 */
typedef struct
{
    /**
     * Number of visible blocks (that are on the ourside of the chunk geometry).
     */
    size_t num_visible_blocks;
    /**
     * Locations within the chunk of all visible blocks
     */
    block_location visible_blocks[SIZE_CHUNK_BLOCKS];

    /**
     * Pre-serialized copy of blocks that can be sent to the GPU.
     */
    gpublock serialized_blocks[SIZE_CHUNK_BLOCKS];
} world_chunk_rendering_t;

/**
 * Structure representing a chunk in the world.
 */
typedef struct
{
    /**
     * Location in Chunk-World-Space of this chunk
     */
    chunk_location location;

    /**
     * If the blocks[] array backing this chunk has
     * been updated but it's index hasn't.
     */
    bool tainted;

    // Metadata for the chunk rendering
    world_chunk_index_t index;
    world_chunk_rendering_t rendering;

    bool  file_previously_existed;
    FILE* persistence;

    /**
     * Flat array of blocks used to represent a chunk state.
     */
    char blocks[SIZE_CHUNK_BLOCKS];
} world_chunk_t;


/**
 * Update the contents of the chunk object.
 *
 * This method is lazy and will only update a chunk that is `tainted`.
 *
 * @param chunk - Chunk to update. Contents will be modified.
 */
void world_chunk_update(world_chunk_t *chunk);

/**
 * Load a chunk into memory. If the chunk does not exist this function
 * will call generate.
 *
 * @param seed - Seed for random noise to be used to generate a world.
 * @param cl - The location of the chunk in chunk-space.
 * @return world_chunk_t* with chunk data or NULL on error.
 */
world_chunk_t *world_chunk_load(long seed, chunk_location cl);

/**
 * Check to see if a chunk has a block at a given location.
 *
 * @param chunk - Chunk to search.
 * @param bl - Location in Block-Chunk-Space.
 * @return true if a block exists in this location.
 */
bool world_chunk_block_location_exists(const world_chunk_t *chunk, const block_location *bl);

/**
 * Check to see if a chunk has a block at a given location.
 *
 * @param chunk - Chunk to search.
 * @param l - Location in Block-World-Space.
 * @return true if the block exists.
 */
bool world_chunk_location_exists(const world_chunk_t *chunk, location *l);

/**
 * Copy the internal chunk data into an array of gpublocks that will be sent
 * into the GPU.
 *
 * @param chunk - Chunk to read data from.
 * @param blocks - Array to write data to.
 * @return Number of gpublock structures written into *blocks.
 */
size_t world_chunk_gpu_send(const world_chunk_t *chunk, gpublock *blocks);

/**
 * Get the height of the world at a given World-Space coordinate
 *
 * @param chunk - Chunk we are inspecting
 * @param l - Location to check
 * @return Height in number of blocks
 */
long world_chunk_height(const world_chunk_t *chunk, location *l);

#endif
