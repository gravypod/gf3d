#ifndef GF3D_COORDS_H
#define GF3D_COORDS_H

#define SIZE_CHUNK_X  16
#define SIZE_CHUNK_Y 255
#define SIZE_CHUNK_Z  16


/**
 * Location of something in world space aligned to a block. Blocks are LONG_MIN to LONG_MAX
 */
typedef struct {
    long long x, y, z;
} location;


/**
 * Location of a chunk's starting position.
 */
typedef struct {
    long x, z;
} chunk_location;

/**
 * Offset within a chunk of a given block
 */
typedef struct {
    long x, y, z;
} block_location;

void location_to_block_location(const location *l, block_location *bl);

long block_location_to_index(const block_location *bl);
void location_from_chunk_block(const chunk_location *cl, block_location *bl, location *l);
void location_to_chunk_location(const location *l, chunk_location *cl);

#endif
