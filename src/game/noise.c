#include "noise.h"

bool noise_block_exists(long seed, long x, long y, long z)
{
    // Chosen by random dice roll
    if (z < x + y) {
        return true;
    }

    return false;
}
