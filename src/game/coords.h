#ifndef GF3D_COORDS_H
#define GF3D_COORDS_H

#include <linmath.h>

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

/**
 * Convert a position to a location.
 *
 * @param position
 * @param l
 */
void position_to_location(const vec3 position, location *l);

/**
 * Take a World-Space location and convert it to a Block-Chunk-Space coordinate.
 *
 * @param l - Location x,y,z tuple in world space without concern for chunk division
 * @param bl - The x,y,z index
 */
void location_to_block_location(const location *l, block_location *bl);

/**
 * Turn a Chunk-Block-Space to Chunk-Array-Index
 *
 * @param bl - Chunk-Block-Space location to convert.
 * @return Index within the the chunk internal char[] of this block
 */
long block_location_to_index(const block_location *bl);

/**
 * Turn Chunk-World-Space location and a Block-Chunk-Space location to a World-Space location.
 *
 * @param cl - Chunk-World-Space
 * @param bl - Block-Chunk-Space
 * @param l - World-Space
 */
void location_from_chunk_block(const chunk_location *cl, block_location *bl, location *l);


/**
 * Turn a World-Space into a Chunk-World-Space coordinate.
 *
 * @param l - World-Space location.
 * @param cl - Chunk-World-Space location.
 */
void location_to_chunk_location(const location *l, chunk_location *cl);

#endif
