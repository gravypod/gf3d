#include <stdlib.h>
#include "structures/mdarray.h"
#include "coords.h"

const array3d chunk_array_layout = {
        .size_x = SIZE_CHUNK_X,
        .size_y = SIZE_CHUNK_Y,
        .size_z = SIZE_CHUNK_Z,
};


void location_to_block_location(location *l, block_location *bl)
{
    bl->x = labs(l->x % SIZE_CHUNK_X);
    bl->y = l->y % SIZE_CHUNK_Y;
    bl->z = labs(l->z % SIZE_CHUNK_Z);
}

void location_to_chunk_location(location *l, chunk_location *cl)
{
    cl->x = l->x / SIZE_CHUNK_X;
    cl->z = l->z / SIZE_CHUNK_Z;
}

void location_from_chunk_block(chunk_location *cl, block_location *bl, location *l)
{
    l->y = bl->y;

    l->x = (cl->x * SIZE_CHUNK_X) + bl->x;
    l->z = (cl->z * SIZE_CHUNK_Z) + bl->z;
}

long block_location_to_index(block_location *bl)
{
    return mdarray_3d_to_1d(&chunk_array_layout, bl->x, bl->y, bl->z);
}