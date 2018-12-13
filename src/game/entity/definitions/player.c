#include <gf3d_vgraphics.h>
#include <gf3d_camera.h>
#include <gf3d_swapchain.h>
#include <game/coords.h>
#include <game/raytrace.h>
#include <game/entity/manager.h>
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


void entity_player_mine(float player_attack_distance, vec3 position, vec3 direction)
{
    const float attack_raytrace_step_size = 0.5f;
    vec3 copy_position;
    vec3 copy_direction;
    memcpy(copy_position, position, sizeof(vec3));
    memcpy(copy_direction, direction, sizeof(vec3));
    location hit_block;
    chunk_location cl;
    block_location bl;
    long bidx;

    if (!raytrace_contacts_block(&hit_block, attack_raytrace_step_size, &player_attack_distance, copy_position, copy_direction)) {
        return;
    }

    location_to_chunk_location(&hit_block, &cl);
    location_to_block_location(&hit_block, &bl);
    bidx = block_location_to_index(&bl);

    world_chunk_t *chunk = world_chunk_get(&cl);

    if (chunk) {
        chunk->blocks[bidx] = 0;
        chunk->tainted = true;
    }
}

void entity_player_attack_entities(entity_t *self, float player_attack_distance, vec3 position, vec3 direction)
{
    const float attack_raytrace_step_size = 0.5f;
    vec3 copy_position;
    vec3 copy_direction;
    memcpy(copy_position, position, sizeof(vec3));
    memcpy(copy_direction, direction, sizeof(vec3));

    entity_t *const contacted = raytrace_contacts_entity(self, attack_raytrace_step_size, &player_attack_distance, copy_position, copy_direction);
    if (!contacted) {
        return;
    }

    printf("Attacked!!! --- %zu\n", contacted->id);
    contacted->health -= 1;

    if (contacted->health > 0) {
        return;
    }

    printf("Killed!!! --- %zu\n", contacted->id);
    entity_manager_release(contacted);
}

void entity_player_position_movement_calculate(const Uint8 *key_states, entity_t *const entity)
{
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
    static const size_t  num_mining_cooldown = 150;
    static size_t mining_cooldown = num_mining_cooldown;

    const Uint8 *key_states = SDL_GetKeyboardState(NULL);
    const bool is_player_attacking = key_states[SDL_SCANCODE_SPACE];
    const bool is_player_mining = key_states[SDL_SCANCODE_X];

    entity_player_position_movement_calculate(key_states, entity);
    entity_player_rotation_movement_calculate(entity);

    gf3d_camera_update(camera);

    float max_player_arm_distance = 20.0f;

    if (is_player_attacking) {
        entity_player_attack_entities(entity, max_player_arm_distance, entity->position, camera->front);
    }

    if (mining_cooldown >= num_mining_cooldown) {
        if (is_player_mining) {
            entity_player_mine(max_player_arm_distance, entity->position, camera->front);
            mining_cooldown = 0;
        }
    } else {
        mining_cooldown += 1;
    }
}

void entity_player_draw(entity_t *entity, const entity_render_pass_t *const pass)
{
}