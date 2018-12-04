#include <gf3d_uniforms.h>
#include <gf3d_vgraphics.h>
#include "agumon.h"


void entity_agumon_init(entity_t *entity, void *metadata)
{
    entity->touching = entity_agumon_touch;
    entity->model = gf3d_model_load("agumon");
}

void entity_agumon_touch(entity_t *entity, entity_t *them)
{
    printf("%zu colliding with %zu\n", entity->id, them->id);
}
