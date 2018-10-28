#include <malloc.h>
#include "manager.h"

struct
{
	size_t freed_entities;

	size_t num_entities;
	size_t next_allocation_index;
	entity_t *entities;
} entity_pool;


bool entity_manager_init()
{
	entity_pool.freed_entities = 0;

	entity_pool.num_entities = MAX_NUM_ENTITIES;
	entity_pool.next_allocation_index = 0;
	entity_pool.entities = calloc(sizeof(entity_t), MAX_NUM_ENTITIES);

	for (size_t i = 0; i < entity_pool.num_entities; i++) {
		entity_pool.entities[i].id = i;
	}

	// Pre-allocate null entities
	entity_manager_for_each(entity_init_empty, NULL, false);

	return true;
}

entity_t *entity_manager_take(entity_initializer_t initializer, void *metadata)
{
	entity_t *entity = NULL;

	if (entity_pool.freed_entities) {
		// We have non-compacted entities. We need to linearly search rather
		// than allocating a new entity
		for (size_t i = 0; i < entity_pool.num_entities; i++) {
			if (entity_pool.entities[i].allocated) {
				continue;
			}

			entity = &entity_pool.entities[i];
			break;
		}

		if (!entity) {
			printf("No entities were free despite freed_entities being marked as %zu\n", entity_pool.freed_entities);
			return NULL;
		}

		entity_pool.freed_entities -= 1;
	} else {
		// All of our entities are compactly allocated. We can take from the end
		// of our preallocated set and avoid a linear search
		if (entity_pool.num_entities < entity_pool.next_allocation_index) {
			// TODO: Slab allocation?
			printf("Ran out of entities to allocate\n");
			return NULL;
		}
		entity = &entity_pool.entities[entity_pool.next_allocation_index++];
	}

	entity->allocated = true;

	// Call initializer
	initializer(entity, metadata);

	return entity;
}


void entity_manager_for_each(entity_consumer_t consumer, void *metadata, bool only_allocated)
{
	size_t remaining_freed_entities = entity_pool.freed_entities;

	for (size_t i = 0; i < entity_pool.num_entities; i++) {

		// If we only want to run against allocated entities stop
		// as soon as we find a non allocated entity
		if (only_allocated) {
			if (!entity_pool.entities[i].allocated) {

				// We expected some freed entities. If we have already hit all of the freed entities
				// that we knew about then we can finally die
				if (!remaining_freed_entities) {
					return;
				}

				// Otherwise we need to mark that we've seen a freed entity and it skip this one
				remaining_freed_entities -= 1;
				continue;
			}
		}

		// Pass to consumer
		consumer(&entity_pool.entities[i], metadata);
	}
}


/**
 * Return an entity to the unallocated pool
 * @param entity
 */
void entity_manager_release(entity_t *entity)
{
	entity_free(entity, NULL);
	entity_init_empty(entity, NULL);

	entity_pool.freed_entities += 1;
}

void entity_manager_update()
{
	entity_manager_for_each(entity_update, NULL, true);
}

void entity_manager_draw(Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	entity_render_pass_t pass;
	pass.commandBuffer = commandBuffer;
	pass.bufferFrame = bufferFrame;
	entity_manager_for_each((entity_consumer_t) entity_draw, &pass, true);
}
