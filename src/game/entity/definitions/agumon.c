#include "agumon.h"


void entity_agumon_init(entity_t *entity, void *metadata)
{
    entity->draw = (void (*)(struct entity_struct *, const entity_render_pass_t *const)) entity_agumon_draw;
    entity->update = (void (*)(struct entity_struct *)) entity_agumon_update;
    entity->free = (void (*)(struct entity_struct *)) entity_agumon_free;
    entity->model = gf3d_model_load("agumon");
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
    gf3d_model_draw(e->model, pass->bufferFrame, pass->commandBuffer);
}