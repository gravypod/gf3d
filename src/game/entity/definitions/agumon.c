#include <gf3d_uniforms.h>
#include <gf3d_vgraphics.h>
#include "agumon.h"


void entity_agumon_init(entity_t *entity, void *metadata)
{
    entity->draw = (void (*)(struct entity_struct *, const entity_render_pass_t *const)) entity_agumon_draw;
    entity->update = (void (*)(struct entity_struct *)) entity_agumon_update;
    entity->free = (void (*)(struct entity_struct *)) entity_agumon_free;
    entity->model = gf3d_model_load("agumon");

    // Locate this entity's UBO
    entity->ubo = gf3d_uniforms_reference_local_get(gf3d_vgraphics_get_instance_uniform_buffer_manager(), (uint32_t) entity->id);

    entity->scale[0] = 1.0f;
    entity->scale[1] = 1.0f;
    entity->scale[2] = 1.0f;
}

void entity_agumon_update(entity_t *entity, void *metadata)
{
    mat4x4_identity(entity->ubo->model);
    mat4x4_translate(entity->ubo->model, entity->position[0], entity->position[1], entity->position[2]);

    mat4x4_rotate_X(entity->ubo->model, entity->ubo->model, entity->rotation[0]);
    mat4x4_rotate_Y(entity->ubo->model, entity->ubo->model, entity->rotation[1]);
    mat4x4_rotate_Z(entity->ubo->model, entity->ubo->model, entity->rotation[2]);

    mat4x4_scale_aniso(entity->ubo->model, entity->ubo->model, entity->scale[0], entity->scale[1], entity->scale[2]);
}

void entity_agumon_free(entity_t *entity, void *metadata)
{
    // TODO: Free any allocated resources
}

void entity_agumon_draw(entity_t *e, entity_render_pass_t *pass)
{
    gf3d_model_draw((uint32_t) e->id, pass->bufferFrame, e->model, pass->bufferFrame, pass->commandBuffer);
}