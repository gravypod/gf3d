#ifndef GF3D_BLOCK_H
#define GF3D_BLOCK_H

#include <stdint.h>

typedef struct
{
    char id;
} block_t;

extern block_t *block_grass, *block_stone;

#endif
