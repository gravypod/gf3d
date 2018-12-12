#ifndef GF3D_BPRUNE_H
#define GF3D_BPRUNE_H

#include "../chunk.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * Prune not-visible block data from chunk structure.
 *
 * This function creates a list of positions that are
 * visible/external parts of a chunk. This means internal
 * blocks (that cannot be seen from any direction) are
 * removed from this structure.
 *
 * @param chunk - Chunk we want to update the index of.
 * @return Number of visible blocks.
 */
size_t bprune_update_chunk_index(world_chunk_t *chunk);

#endif
