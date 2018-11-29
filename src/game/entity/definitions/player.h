#ifndef GF3D_PLAYER_H
#define GF3D_PLAYER_H

#include "../entity.h"

extern entity_t *player_entity;

/**
 * Initialize an player
 *
 * @param entity - The entity struct to fill in
 * @param metadata - Metadata that can be passed from the caller. Unused in init()
 */
void entity_player_init(entity_t *entity, void *metadata);

/**
 * Update an player
 *
 * @param entity - The entity struct to update
 * @param metadata - Metadata that can be passed from the caller. Unused in update()
 */
void entity_player_update(entity_t *entity, void *metadata);

/**
 * Free an player
 *
 * @param entity - Entity to free custom data from
 * @param metadata - Metadata that can be passed from the caller. Unused in free()
 */
void entity_player_free(entity_t *entity, void *metadata);

/**
 * Draw an player
 *
 * @param e - Entity to draw
 * @param pass - Metadata about the current render pass that we are drawing
 */
void entity_player_draw(entity_t *e, entity_render_pass_t *pass);


#endif
