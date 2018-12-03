#ifndef GF3D_MDARRAY_H
#define GF3D_MDARRAY_H

#include <stddef.h>

typedef struct {
    long size_x, size_y, size_z;
} array3d;

static inline long mdarray_3d_to_1d(const array3d * const self, long x, long y, long z)
{
    return (x * self->size_z * self->size_y) + (z * self->size_y) + y;
}

/*
static inline void mdarray_1d_to_3d(array3d *self, long idx, long *x, long *y, long *z)
{
    *z = idx / (self->size_x * self->size_y);
    idx -= (*z * self->size_x * self->size_y);
    *y = idx / self->size_x;
    *x = idx % self->size_x;
}
*/


#endif
