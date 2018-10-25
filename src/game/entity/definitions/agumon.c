#include <gf3d_uniforms.h>
#include "agumon.h"


void entity_agumon_init(entity_t *entity, void *metadata)
{
    entity->draw = (void (*)(struct entity_struct *, const entity_render_pass_t *const)) entity_agumon_draw;
    entity->update = (void (*)(struct entity_struct *)) entity_agumon_update;
    entity->free = (void (*)(struct entity_struct *)) entity_agumon_free;
    entity->model = gf3d_model_load("agumon");

    entity->position = vector3d(0, 0, 0);
    entity->scale = vector3d(1, 1, 1);
    entity->rotation = vector3d(0, 0, 0);
}

void entity_agumon_update(entity_t *entity, void *metadata)
{

}

void entity_agumon_free(entity_t *entity, void *metadata)
{
    // TODO: Free any allocated resources
}

void entity_agumon_draw(entity_t *e, entity_render_pass_t *pass)
{
    UniformBufferObject *ubo = gf3d_uniforms_buffers_reference_get(pass->bufferFrame, e->id);
    gf3d_matrix_identity(ubo->model);
    gf3d_matrix_make_translation(ubo->model, e->position);

    gf3d_model_draw(e->model, pass->bufferFrame, e->id, pass->commandBuffer);
}