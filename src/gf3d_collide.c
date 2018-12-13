#include "game/entity/manager.h"
#include <gf3d_collide.h>
#include <game/shapes/box.h>

static inline float gf3d_collide_entity_magnitude_min(const entity_t *const e, mesh_dimenstions_t *b)
{
    return fminf(fminf(b->x_min * e->scale[0], b->y_min * e->scale[1]), b->z_min * e->scale[2]);
}

static inline float gf3d_collide_entity_magnitude_max(const entity_t *const e, mesh_dimenstions_t *b)
{
    return fmaxf(fmaxf(b->x_max * e->scale[0], b->y_max * e->scale[1]), b->z_max * e->scale[2]);
}

static inline float gf3d_collide_entity_radius(const entity_t *const e, mesh_dimenstions_t *b)
{
    return fabsf(gf3d_collide_entity_magnitude_max(e, b) - gf3d_collide_entity_magnitude_min(e, b));
}


bool gf3d_collide_is_point_touching(const float point_radius, const vec3 point, const entity_t *const b)
{
    if (!b || !b->model || !b->model->mesh) {
        return false;
    }

    const float
            ax = point[0] - point_radius,
            ay = point[1] - point_radius,
            az = point[2] - point_radius,

            bx = b->position[0] - b->bb.offset_x,
            by = b->position[1] - b->bb.offset_y,
            bz = b->position[2] - b->bb.offset_z;


    const float
            dx = fabsf(ax - bx),
            dy = fabsf(ay - by),
            dz = fabsf(az - bz);

    const float
            rx = fmaxf(point_radius, b->bb.radius_x),
            ry = fmaxf(point_radius, b->bb.radius_y),
            rz = fmaxf(point_radius, b->bb.radius_z);


    if (dx < rx) {
        if (dy < ry) {
            if (dz < rz) {
                return true;
            }
        }
    }

    return false;
}

bool gf3d_collide_is_entity_touching(const entity_t *const a, const entity_t *const b)
{
    if (!a || !b || !a->model || !a->model->mesh || !b->model || !b->model->mesh) {
        return false;
    }

    const float
            ax = a->position[0] - a->bb.offset_x,
            ay = a->position[1] - a->bb.offset_y,
            az = a->position[2] - a->bb.offset_z,

            bx = b->position[0] - b->bb.offset_x,
            by = b->position[1] - b->bb.offset_y,
            bz = b->position[2] - b->bb.offset_z;


    const float
            dx = fabsf(ax - bx),
            dy = fabsf(ay - by),
            dz = fabsf(az - bz);

    const float
            rx = fmaxf(a->bb.radius_x, b->bb.radius_x),
            ry = fmaxf(a->bb.radius_z, b->bb.radius_y),
            rz = fmaxf(a->bb.radius_y, b->bb.radius_z);


    if (dx < rx) {
        if (dy < ry) {
            if (dz < rz) {
                return true;
            }
        }
    }

    return false;
}



