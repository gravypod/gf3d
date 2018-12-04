#ifndef ENGINE_ENTITY_ENTITY_H
#define ENGINE_ENTITY_ENTITY_H

#include <linmath.h>
#include <stddef.h>
#include <stdbool.h>
#include <SDL_quit.h>
#include <vulkan/vulkan.h>
#include <gf3d_uniforms.h>
#include "gf3d_model.h"

/**
 * Definition of a render pass from the game engine. Contains information
 * needed to correctly draw a scene.
 */
typedef struct
{
    /**
     * The image in the swap chain we are rendering to.
     */
    Uint32 bufferFrame;
    /**
     * The command buffer we are using to render
     */
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
    size_t id;

    /**
     * Movement speed of an entity
     */
    float speed;

    /**
     * The location in world space of the entity
     */
    vec3 position;
    /**
     * The scale in world space of the entity
     */
    vec3 scale;
    /**
     * The rotation, per axis, in world space of the entity.
     */
    vec3 rotation;

    /**
     * Pointer to a Model. This defines the information
     * used to render the entity combining a mesh and a texture.
     */
    Model *model;

    /**
     * The UBO allocated to this entity.
     */
    InstanceUniformBufferObject *ubo;

    /**
     * If this entity is already being used by something
     */
    bool allocated;

    /**
     * Called to free this entity.
     */
    void (*free)(struct entity_struct *, void *);

    /**
     * Called to update this entity
     */
    void (*update)(struct entity_struct *, void *);

    /**
     * Called when touching another entity
     */
    void (*touching)(struct entity_struct *, struct entity_struct *);

    /**
     * Called to draw this entity
     * @param render_pass
     */
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
 * Initialize an empty entity. This is used to reset an entity to an unused state.
 *
 * @param e - Entity to blank out
 */
void entity_init_empty(entity_t *e, void *metadata);

/**
 * Post-init function for entities
 * @param entity
 */
void entity_post_init(entity_t *entity);

/**
 * Called when touching another entity
 * @param a - Self reference
 * @param b - Entity being touched
 */
void entity_touch(entity_t *a, entity_t *b);

/**
 * Update a generic entity.
 *
 * @param entity - Entity to update
 * @param metadata - Metadata to pass to internal function
 */
void entity_update(entity_t *entity, void *metadata);

/**
 * Free an entity.
 *
 * @param entity - Entity to free
 * @param metadata - Metadata to pass to entity function
 */
void entity_free(entity_t *entity, void *metadata);

/**
 * Draw an entity to a command buffer
 *
 * @param e - Entity to draw
 * @param render_pass - The render pass metadata to send to the entity.
 */
void entity_draw(entity_t *e, const entity_render_pass_t *render_pass);

#endif
