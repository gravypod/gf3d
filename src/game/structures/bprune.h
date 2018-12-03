#ifndef GF3D_BPRUNE_H
#define GF3D_BPRUNE_H

#include "../chunk.h"
#include <stddef.h>
#include <stdbool.h>

block_location *bprune_chunk(const world_chunk_t *chunk, size_t *num_blocks);

#endif
