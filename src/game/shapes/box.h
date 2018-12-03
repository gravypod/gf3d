#ifndef GF3D_BOX_H
#define GF3D_BOX_H

#include <linmath.h>
#include <stdbool.h>

typedef struct {
    struct {
        float x, y, z;
    } bottom_back_left;
    float x_size;
    float y_size;
    float z_size;
} box3d;

/**
 * Test to see if two boxes are colliding.
 *
 * @param left
 * @param right
 * @return true if the boxes are colliding
 */
bool box3d_colliding(const box3d *left, const box3d *right);

#endif
