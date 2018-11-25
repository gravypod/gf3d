#ifndef GF3D_AGUMON_H
#define GF3D_AGUMON_H


#include "../manager.h"

/**
 * Initialize an Agumon
 *
 * @param entity - The entity struct to fill in
 * @param metadata - Metadata that can be passed from the caller. Unused in init()
 */
void entity_agumon_init(entity_t *entity, void *metadata);

/**
 * Update an Agumon
 *
 * @param entity - The entity struct to update
 * @param metadata - Metadata that can be passed from the caller. Unused in update()
 */
void entity_agumon_update(entity_t *entity, void *metadata);


/**
 * Touch an entity
 *
 * @param entity - The entity struct to update
 * @param them - Entity we are touching
 */
void entity_agumon_touch(entity_t *entity, entity_t *them);

/**
 * Free an Agumon
 *
 * @param entity - Entity to free custom data from
 * @param metadata - Metadata that can be passed from the caller. Unused in free()
 */
void entity_agumon_free(entity_t *entity, void *metadata);

/**
 * Draw an Agumon
 *
 * @param e - Entity to draw
 * @param pass - Metadata about the current render pass that we are drawing
 */
void entity_agumon_draw(entity_t *e, entity_render_pass_t *pass);


#endif
