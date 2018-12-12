#include <gf3d_uniforms.h>
#include <gf3d_vgraphics.h>
#include "agumon.h"
#include <game/entity/definitions/player.h>

void entity_agumon_update(entity_t *entity, void *metadata)
{
    float xdiff = player_entity->position[0] - entity->position[0];
    float zdiff = player_entity->position[2] - entity->position[2] ;

    entity->position[0] += xdiff * entity->speed;
    entity->position[2] += zdiff * entity->speed;
}

void entity_agumon_init(entity_t *entity, void *metadata)
{
    const entity_block_bounding_box_t bb = {
            .radius_x = 0.5f,
            .radius_y = 0.5f,
            .radius_z = 0.5f,

            .offset_x = 0.5f,
            .offset_y = 0.5f,
            .offset_z = 0.5f,
    };

    entity->speed = 0.005f;

    entity->scale[0] = 0.08f;
    entity->scale[1] = 0.08f;
    entity->scale[2] = 0.08f;

    entity->rotation[0] = 180.0f;

    entity->bb = bb;

    entity->touching = entity_agumon_touch;
    entity->update = entity_agumon_update;
    entity->model = gf3d_model_load("agumon");
}

void entity_agumon_touch(entity_t *entity, entity_t *them)
{
    printf("%zu colliding with %zu\n", entity->id, them->id);
}
