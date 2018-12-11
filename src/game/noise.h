#ifndef GF3D_NOISE_H
#define GF3D_NOISE_H

#include <stdbool.h>
#include "block.h"

/**
 * Check to see if a block should exist using a seed.
 *
 * @param seed
 * @param x
 * @param y
 * @param z
 * @return
 */
bool noise_block_exists(long seed, long x, long y, long z);

/**
 * Get the type of a block at a specific location.
 *
 * @param seed - World seed.
 * @param x - X position in World-Space.
 * @param y - Y position in World-Space.
 * @param z - Z position in World-Space.
 * @return Pointer to block definition.
 */
block_t *noise_block_type(long seed, long x, long y, long z);

#endif
