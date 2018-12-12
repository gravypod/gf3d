#include <gf3d_uniforms.h>
#include <gf3d_vgraphics.h>
#include "badguy.h"
#include <game/entity/definitions/player.h>
#include <game/game.h>

void entity_badguy_update(entity_t *entity, void *metadata)
{
    float xdiff = player_entity->position[0] - entity->position[0];
    float zdiff = player_entity->position[2] - entity->position[2] ;

    entity->position[0] += xdiff * entity->speed;
    entity->position[2] += zdiff * entity->speed;
}

void entity_badguy_init(entity_t *entity, void *metadata)
{
    const entity_block_bounding_box_t bb = {
            .radius_x = 1.0f,
            .radius_y = 1.0f,
            .radius_z = 1.0f,

            .offset_x = 0.5f,
            .offset_y = 0.5f,
            .offset_z = 0.5f,
    };

    entity->speed = 0.005f;

    entity->scale[0] = 0.08f;
    entity->scale[1] = 0.08f;
    entity->scale[2] = 0.08f;

    entity->rotation[0] = 70.0f;

    entity->bb = bb;

    entity->touching = entity_badguy_touch;
    entity->update = entity_badguy_update;
    entity->model = gf3d_model_load("agumon");
}

void entity_badguy_touch(entity_t *entity, entity_t *them)
{
    if (them == player_entity) {
        if (player_entity->health > 0) {
            player_entity->health--;
            printf("Adding player health (%li)\n", player_entity->health);
        } else {
            game_keep_running = false;
        }
    }
}
