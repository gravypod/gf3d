#include <gf3d_vgraphics.h>
#include <game/entity/definitions/world.h>
#include "entity.h"


void entity_update_gravity(entity_t *entity)
{
    location l;
    position_to_location(entity->position, &l);
    long h = world_height(&l);

    if (h <= 0) {
        return;
    }

    const float height_of_floor = h + entity->bb.offset_y + 1.01f;

    float height_of_entity = entity->position[1];

    if (height_of_entity > height_of_floor) {
        height_of_entity -= 0.04f;
    }

    if (height_of_entity < height_of_floor) {
        height_of_entity = height_of_floor;
    }

    entity->position[1] = height_of_entity;
}

void entity_init_empty(entity_t *e, void *metadata)
{
    (void) metadata;
    if (!e) {
        return;
    }

    memset(&e->bb, 0, sizeof(e->bb));

    e->allocated = false;

    // Null out behavior
    e->model = NULL;
    e->draw = NULL;
    e->free = NULL;
    e->touching = NULL;
    e->update = NULL;

    e->position[0] = 0.0f;
    e->position[1] = 0.0f;
    e->position[2] = 0.0f;

    e->scale[0] = 1.0f;
    e->scale[1] = 1.0f;
    e->scale[2] = 1.0f;
}

void entity_post_init(entity_t *entity)
{
    if (entity->model) {
        // Locate this entity's UBO
        entity->ubo = gf3d_uniforms_reference_local_get(gf3d_vgraphics_get_instance_uniform_buffer_manager(), (uint32_t) entity->id);
    }

    entity->allocated = true;
}

void entity_update(entity_t *entity, void *metadata)
{
    (void) metadata;
    if (entity->update) {
        entity->update(entity, metadata);
    }

    entity_update_gravity(entity);

    if (entity->model) {
        mat4x4_identity(entity->ubo->model);
        mat4x4_translate(entity->ubo->model, entity->position[0], entity->position[1], entity->position[2]);

        mat4x4_rotate_X(entity->ubo->model, entity->ubo->model, entity->rotation[0]);
        mat4x4_rotate_Y(entity->ubo->model, entity->ubo->model, entity->rotation[1]);
        mat4x4_rotate_Z(entity->ubo->model, entity->ubo->model, entity->rotation[2]);

        mat4x4_scale_aniso(entity->ubo->model, entity->ubo->model, entity->scale[0], entity->scale[1], entity->scale[2]);
    }
}

void entity_free(entity_t *entity, void *metadata)
{
    (void) metadata;

    if (entity->model) {
        gf3d_model_free(entity->model);
    }

    if (entity->free) {
        entity->free(entity, metadata);
    }
}

void entity_draw(entity_t *e, const entity_render_pass_t *const render_pass)
{
    if (e->draw) {
        e->draw(e, render_pass);
    } else if (e->model) {
        gf3d_model_draw(
                (uint32_t) e->id,
                render_pass->bufferFrame,
                e->model,
                render_pass->bufferFrame,
                render_pass->commandBuffer
        );
    }
}

void entity_touch(entity_t *a, entity_t *b)
{
    if (a->touching) {
        a->touching(a, b);
    }
}