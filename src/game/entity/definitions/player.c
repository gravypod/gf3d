#include <gf3d_vgraphics.h>
#include <gf3d_camera.h>
#include <gf3d_swapchain.h>
#include <game/coords.h>
#include "player.h"
#include "world.h"

entity_t *player_entity;
gf3d_camera *camera;

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
            entity_player_speed_when_clicking(entity, w, s),
            entity_player_speed_when_clicking(entity, d, a),
            0.0f
    };
    vec3_scale(delta, delta, entity->speed);

    // Move the player camera by these metrics
    gf3d_camera_move(camera, delta[0], delta[1], delta[2]);
}

void entity_player_rotation_movement_calculate(entity_t *const entity)
{
    int changeX, changeY;

    SDL_GetRelativeMouseState(&changeX, &changeY);

    float yaw = ((float) changeX) * 0.4f;
    float pitch = ((float) changeY) * -0.3f;

    gf3d_camera_turn(camera, pitch, yaw);
}

void entity_player_touch(entity_t *self, entity_t *obj)
{
    printf("Player touched %zu\n", obj->id);
}

void entity_player_init(entity_t *entity, void *metadata)
{
    const entity_block_bounding_box_t bb = {
            .radius_x = 1.0f,
            .radius_y = 1.0f,
            .radius_z = 1.0f,

            .offset_x = 0.5f,
            .offset_y = 3.0f,
            .offset_z = 0.5f
    };

    entity->bb = bb;
    entity->update = entity_player_update;
    entity->touching = entity_player_touch;
    entity->draw = entity_player_draw;
    entity->model = gf3d_model_load("agumon");
    entity->speed = 0.1f;
    entity->position[0] -= 1.0f;
    camera = gf3d_camera_init(windowWidth, windowHeight, entity->position, entity->rotation);
    player_entity = entity;
}


void entity_player_update(entity_t *entity, void *metadata)
{
    entity_player_position_movement_calculate(entity);
    entity_player_rotation_movement_calculate(entity);

    gf3d_camera_update(camera);
}

void entity_player_draw(entity_t *entity, const entity_render_pass_t *const pass)
{
}