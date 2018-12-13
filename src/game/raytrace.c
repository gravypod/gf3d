#include "raytrace.h"
#include <stddef.h>
#include <gf3d_collide.h>
#include <game/entity/manager.h>

bool raytrace_point_step(float step_size, float *max_distance, vec3 point, vec3 direction)
{
    vec3 step_direction;

    if (max_distance == NULL || *max_distance < step_size) {
        return false;
    }

    vec3_norm(step_direction, direction);
    vec3_scale(step_direction, step_direction, step_size);

    vec3_add(point, point, step_direction);

    *max_distance -= step_size;

    return true;
}

entity_t *raytrace_contacts_entity(entity_t *source_entity, float step_size, float *max_distance, vec3 point, vec3 direction)
{
    while (raytrace_point_step(step_size, max_distance, point, direction)) {
        size_t last_entity_id = 0;
        entity_t *entity = NULL;

        while (entity_manager_iterate_generator(&last_entity_id, true, &entity)) {
            if (gf3d_collide_is_point_touching(0.05f, point, entity)) {
                if (source_entity != NULL && source_entity->id == entity->id) {
                    continue;
                }
                return entity;
            }
        }
    }

    return NULL;
}
