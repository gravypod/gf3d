#include "world.h"
#include <game/entity/definitions/world.h>


bool collisions_world_blocks_occupied_generator(
        const vec3 center,
        float x_radius, float y_radius, float z_radius,
        location *block_location
)
{
    const vec3 back_bottom_left = {center[0] - x_radius, center[1] - y_radius, center[2] - z_radius};
    const vec3 top_front_right = {center[0] + x_radius, center[1] + y_radius, center[2] + z_radius};

    if (block_location->x == -1 && block_location->y == -1 && block_location->z == -1) {
        block_location->x = (long) back_bottom_left[0];
        block_location->y = (long) back_bottom_left[1];
        block_location->z = (long) back_bottom_left[2];
    }

    if (block_location->x < ceilf(top_front_right[0])) {
        if (block_location->y < ceilf(top_front_right[1])) {
            if (block_location->z < ceilf(top_front_right[2])) {
                block_location->z++;
                return true;
            }

            block_location->z = (long) back_bottom_left[2];
            block_location->y++;
            return true;
        }

        block_location->z = (long) back_bottom_left[2];
        block_location->y = (long) back_bottom_left[1];
        block_location->x++;
        return true;
    }

    return false;
}

bool collisions_is_below_world(const entity_t *entity)
{
    const entity_block_bounding_box_t *const bb = &entity->bb;
    const float rx = bb->radius_x,
            ry = bb->radius_y,
            rz = bb->radius_z;

    vec3 position;
    memcpy(position, entity->position, sizeof(vec3));
    position[1] -= entity->bb.offset_x;

    location l = {
            .x = -1,
            .y = -1,
            .z = -1,
    };
    while (collisions_world_blocks_occupied_generator(position, rx, ry, rz, &l)) {
        if (world_height(&l) >= l.y) {
            return true;
        }
    }

    return false;
}
