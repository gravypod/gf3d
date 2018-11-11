#include <gf3d_vgraphics.h>
#include <gf3d_camera.h>
#include <gf3d_swapchain.h>
#include "player.h"

const float entity_player_speed_when_clicking(const entity_t *const entity, const bool positive, const bool negative)
{
    if (positive) {
        return 1;
    }

    if (negative) {
        return -1;
    }

    return 0;
}

void entity_player_position_movement_calculate(entity_t *const entity)
{
    const Uint8 *key_states = SDL_GetKeyboardState(NULL);

    // Keys we are listening to direction keys
    const bool w = key_states[SDL_SCANCODE_W]; // Forward
    const bool s = key_states[SDL_SCANCODE_S]; // Backward

    const bool d = key_states[SDL_SCANCODE_D]; // Left
    const bool a = key_states[SDL_SCANCODE_A]; // Right

    vec3 delta = {
            entity_player_speed_when_clicking(entity, d, a),
            entity_player_speed_when_clicking(entity, w, s),
            0.0f
    };
    vec3_scale(delta, delta, entity->speed);

    vec3_add(entity->position, entity->position, delta);
    gf3d_camera_move(delta);
}


void entity_player_init(entity_t *entity, void *metadata)
{
    entity->update = (void (*)(struct entity_struct *)) entity_player_update;
    entity->speed = 0.1f;
}

void entity_player_update(entity_t *entity, void *metadata)
{
    entity_player_position_movement_calculate(entity);
}
