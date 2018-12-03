#ifndef GF3D_CHUNK_H
#define GF3D_CHUNK_H

#include <stddef.h>
#include <linmath.h>
#include "coords.h"
#include "rendering/world.h"

#define SIZE_CHUNK_BLOCKS  ((SIZE_CHUNK_X) * (SIZE_CHUNK_Y) * (SIZE_CHUNK_Z))

typedef struct
{
    chunk_location location;
    char blocks[SIZE_CHUNK_BLOCKS];
} world_chunk_t;


world_chunk_t *world_chunk_load(long seed, chunk_location cl);
size_t world_chunk_gpu_serialize(world_chunk_t *chunk, gpublock *blocks);

#endif
