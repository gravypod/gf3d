#include "game/entity/manager.h"
#include <gf3d_collide.h>

static inline float gf3d_collide_entity_magnitude_min(mesh_dimenstions_t *b)
{
    return fminf(fminf(b->x_min, b->y_min), b->z_min);
}

static inline float gf3d_collide_entity_magnitude_max(mesh_dimenstions_t *b)
{
    return fmaxf(fmaxf(b->x_max, b->y_max), b->z_max);
}

static inline float gf3d_collide_entity_radius(mesh_dimenstions_t *b)
{
    return fabsf(gf3d_collide_entity_magnitude_max(b) - gf3d_collide_entity_magnitude_min(b));
}

bool gf3d_collide_is_entity_touching(const entity_t *const a, const entity_t *const b)
{
    if (!a || !b || !a->model || !a->model->mesh || !b->model || !b->model->mesh) {
        return false;
    }

    mesh_dimenstions_t *a_dimensions = &a->model->mesh->dimensions;
    mesh_dimenstions_t *b_dimensions = &b->model->mesh->dimensions;

    const float
            ax = a->position[0],
            ay = a->position[1],
            az = a->position[2],
            bx = b->position[0],
            by = b->position[1],
            bz = b->position[2];


    const float a_radius = gf3d_collide_entity_radius(a_dimensions);
    const float b_radius = gf3d_collide_entity_radius(b_dimensions);

    const float delta_x = ax - bx;
    const float delta_y = ay - by;
    const float delta_z = az - bz;

    if ((a_radius * a_radius) + (b_radius + b_radius) <= (delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z)) {
        return false;
    }

    return true;
}



