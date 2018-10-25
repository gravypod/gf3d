#include "entity.h"

void entity_init_empty(entity_t *e, void *metadata)
{
    (void) metadata;
    if (!e) {
        return;
    }

    e->allocated = false;

    // Null out behavior
    e->draw = NULL;
    e->free = NULL;
    e->update = NULL;
}

void entity_update(entity_t *entity, void *metadata)
{
    (void) metadata;
    if (entity->update) {
        entity->update(entity);
    }
}

void entity_free(entity_t *entity, void *metadata)
{
    (void) metadata;
    if (entity->free) {
        entity->free(entity);
    }
}

void entity_draw(entity_t *e, const entity_render_pass_t *const render_pass)
{
    if (e->draw) {
        e->draw(e, render_pass);
    }
}
