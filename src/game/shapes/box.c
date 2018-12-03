#include "box.h"

bool box3d_colliding(const box3d *left, const box3d *right)
{
    const float
            left_x_min = left->bottom_back_left.x, left_x_max = left_x_min + left->x_size,
            left_y_min = left->bottom_back_left.y, left_y_max = left_y_min + left->y_size,
            left_z_min = left->bottom_back_left.z, left_z_max = left_z_min + left->z_size,

            right_x_min = right->bottom_back_left.x, right_x_max = right_x_min + right->x_size,
            right_y_min = right->bottom_back_left.y, right_y_max = right_y_min + right->y_size,
            right_z_min = right->bottom_back_left.z, right_z_max = right_z_min + right->z_size;

    if (left_x_min < right_x_max || left_x_max > right_x_min) {
        if (left_y_min < right_y_max || left_y_max > right_y_min) {
            if (left_z_min < right_z_max || left_z_max > right_z_min) {
                return true;
            }
        }
    }

    return false;
}