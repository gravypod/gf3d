#ifndef GF3D_AGUMON_H
#define GF3D_AGUMON_H



#include "../manager.h"

void entity_agumon_init(entity_t *entity, void *metadata);

void entity_agumon_update(entity_t *entity, void *metadata);

void entity_agumon_free(entity_t *entity, void *metadata);

void entity_agumon_draw(entity_t *e, entity_render_pass_t *pass);


#endif
