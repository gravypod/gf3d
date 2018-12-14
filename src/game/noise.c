#include "noise.h"
#include "coords.h"
#include <open-simplex-noise.h>
#include <stddef.h>

struct osn_context *ctx = NULL;


bool noise_block_exists(long seed, long x, long y, long z)
{
    if (ctx == NULL) {
        open_simplex_noise(seed, &ctx);
    }

    double height = (((SIZE_CHUNK_Y - 40) / 10) * fabs(open_simplex_noise2(ctx, x / ((double) SIZE_CHUNK_X), z / ((double) SIZE_CHUNK_Z)))) + 40;

    // Chosen by random dice roll
    if (y <= height) {
        return true;
    }

    return false;
}

block_t *noise_block_type(long seed, long x, long y, long z)
{
    double type = round(open_simplex_noise3(ctx, x, y, z));

    if (type < 0) {
        return block_stone;
    }

    return block_grass;
}