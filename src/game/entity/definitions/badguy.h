#ifndef GF3D_BADGUY_H
#define GF3D_BADGUY_H


#include "../manager.h"

/**
 * Initialize an badguy
 *
 * @param entity - The entity struct to fill in
 * @param metadata - Metadata that can be passed from the caller. Unused in init()
 */
void entity_badguy_init(entity_t *entity, void *metadata);


/**
 * Touch an entity
 *
 * @param entity - The entity struct to update
 * @param them - Entity we are touching
 */
void entity_badguy_touch(entity_t *entity, entity_t *them);


#endif
