#include "noise.h"

bool noise_block_exists(long seed, long x, long y, long z)
{
    // Chosen by random dice roll
    if (y < x + z) {
        return true;
    }

    return false;
}
