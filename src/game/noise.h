#ifndef GF3D_NOISE_H
#define GF3D_NOISE_H

#include <stdbool.h>

/**
 * Check to see if a block should exist using a seed.
 * @param seed
 * @param x
 * @param y
 * @param z
 * @return
 */
bool noise_block_exists(long seed, long x, long y, long z);

#endif
