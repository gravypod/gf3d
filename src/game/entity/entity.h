#ifndef ENGINE_ENTITY_ENTITY_H
#define ENGINE_ENTITY_ENTITY_H

#include <stddef.h>
#include <stdbool.h>
#include <SDL_quit.h>
#include <vulkan/vulkan.h>
#include "gf3d_model.h"

typedef struct
{
    Uint32 bufferFrame;
    VkCommandBuffer commandBuffer;
} entity_render_pass_t;

/**
 * Abstract definition of an entity
 */
typedef struct entity_struct
{
    /**
     * Unique ID for every entity
     */
    uint32_t id;

    Vector3D position;
    Vector3D scale;
    Vector3D rotation;

    Model *model;

    /**
     * If this entity is already being used by something
     */
    bool allocated;

    void (*free)(struct entity_struct *);

    void (*update)(struct entity_struct *);

    void (*draw)(struct entity_struct *, const entity_render_pass_t *const render_pass);
} entity_t;

/**
 * Abstract definition of initialization of an entity
 */
typedef void (*entity_initializer_t)(entity_t *, void *);

/**
 * Consume and process an entity
 */
typedef void (*entity_consumer_t)(entity_t *, void *);


/**
 * Initialize an empty entity
 * @param e
 */
void entity_init_empty(entity_t *e, void *metadata);


void entity_update(entity_t *entity, void *metadata);

void entity_free(entity_t *entity, void *metadata);

/**
 * Draw an entity to a command buffer
 *
 * @param e
 * @param buffer
 */
void entity_draw(entity_t *e, const entity_render_pass_t *render_pass);

#endif
