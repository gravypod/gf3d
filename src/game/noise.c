#include "noise.h"

bool noise_block_exists(long seed, long x, long y, long z)
{
    // Chosen by random dice roll
    if (y < x + z) {
        return true;
    }

    return false;
}

block_t *noise_block_type(long seed, long x, long y, long z)
{
    if (y < 10) {
        return block_stone;
    }

    return block_grass;
}