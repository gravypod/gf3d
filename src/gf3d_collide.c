#include "game/entity/manager.h"
#include <gf3d_collide.h>

inline float gf3d_collide_entity_magnitude_min(bounding_box_t *b)
{
    return fminf(fminf(b->x_min, b->y_min), b->z_min);
}

inline float gf3d_collide_entity_magnitude_max(bounding_box_t *b)
{
    return fmaxf(fmaxf(b->x_max, b->y_max), b->z_max);
}

inline bool within_reach(float position, float offset, float size)
{
    return position > (offset + size);
}

bool gf3d_collide_is_entity_touching(entity_t *a, entity_t *b)
{
    if (!a || !b || !a->model || !a->model->mesh || !b->model || !b->model->mesh) {
        return false;
    }

    bounding_box_t *a_bounding_box = &a->model->mesh->bounding_box;
    bounding_box_t *b_bounding_box = &b->model->mesh->bounding_box;

    float
            ax = a->position[0],
            ay = a->position[1],
            az = a->position[2],
            bx = b->position[0],
            by = b->position[1],
            bz = b->position[2];

    float a_size_x = fabsf(a_bounding_box->x_max - a_bounding_box->x_min);
    float a_size_y = fabsf(a_bounding_box->z_max - a_bounding_box->y_min);
    float a_size_z = fabsf(a_bounding_box->y_max - a_bounding_box->z_min);

    float b_size_x = fabsf(b_bounding_box->x_max - b_bounding_box->x_min);
    float b_size_y = fabsf(b_bounding_box->z_max - b_bounding_box->y_min);
    float b_size_z = fabsf(b_bounding_box->y_max - b_bounding_box->z_min);

    bool outside_of_aabb =
            within_reach(ax, bx, b_size_x) || // B Checks
            within_reach(ay, by, b_size_y) ||
            within_reach(az, bz, b_size_z) ||
            within_reach(bx, ax, a_size_x) || // A Checks
            within_reach(by, ay, a_size_y) ||
            within_reach(bz, az, a_size_z);

    if (outside_of_aabb)
        return false;

    return true;
}



