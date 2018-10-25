#ifndef ENGINE_ENTITY_MANAGER_H
#define ENGINE_ENTITY_MANAGER_H

#include <stdbool.h>
#include <stddef.h>
#include "entity.h"

#define MAX_NUM_ENTITIES 128

/**
 * Wrap entity_manager_make with casts for simplicity
 */
#define entity_manager_make(initializer, metadata) entity_manager_take((entity_initializer_t) initializer, (void*) metadata)

/**
 * Initialize the entity pool
 * @param num_entites
 * @return
 */
bool entity_manager_init();

/**
 * Find a free entity
 * @param initializer - Initialization function
 * @param metadata - Metadata required to create this entity
 * @return
 */
entity_t *entity_manager_take(entity_initializer_t initializer, void *metadata);

/**
 * Return an entity to the unallocated pool
 * @param entity
 */
void entity_manager_release(entity_t *entity);

/**
 * Run a function against every allocated entity
 * @param consumer
 * @param metadata - Data passed to the consumer in addition to the entity
 * @param only_allocated
 */
void entity_manager_for_each(entity_consumer_t consumer, void *metadata, bool only_allocated);


void entity_manager_update();

void entity_manager_draw(Uint32 bufferFrame, VkCommandBuffer commandBuffer);

#endif
