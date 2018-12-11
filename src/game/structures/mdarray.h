#ifndef GF3D_MDARRAY_H
#define GF3D_MDARRAY_H

#include <stddef.h>

/**
 * Size of the 3d array.
 */
typedef struct
{
    long size_x, size_y, size_z;
} array3d;

/**
 * Turn a 3d position in a matrix into a 1d position in a vector. Used
 * to take 3d world space data and map it into a flat char[] of block
 * data.
 *
 * @param self - Size of the 3d matrix we are projecting from.
 * @param x - X position.
 * @param y - Y position.
 * @param z - Z position.
 * @return Location in a flat array that represents this position.
 */
static inline long mdarray_3d_to_1d(const array3d *const self, long x, long y, long z)
{
    return (x * self->size_z * self->size_y) + (z * self->size_y) + y;
}

#endif
