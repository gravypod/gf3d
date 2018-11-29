#ifndef GF3D_COORDS_H
#define GF3D_COORDS_H


/**
 * Convert a 3D coord to a 1D coord.
 *
 * https://stackoverflow.com/a/34363187
 *
 * @param x
 * @param y
 * @param z
 * @param max_x
 * @param max_y
 * @return
 */
static inline long coord_flatten_3d_to_1d(long x, long y, long z, long max_x, long max_y)
{
    return (z * max_x * max_y) + (y * max_x) + x;
}

/**
 * Convert a 1D coord to a 3D coord.
 * @param idx
 * @param max_x
 * @param max_y
 * @param x
 * @param y
 * @param z
 * @return
 */
static inline void coord_expand_1d_to_3d(long idx, long max_x, long max_y, long *x, long *y, long *z)
{
    *z = idx / (max_x * max_y);
    idx -= (*z * max_x * max_y);
    *y = idx / max_x;
    *x = idx % max_x;
}

#endif
