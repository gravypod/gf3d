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
 * Touch an entity
 *
 * @param entity - The entity struct to update
 * @param them - Entity we are touching
 */
void entity_agumon_touch(entity_t *entity, entity_t *them);


#endif
