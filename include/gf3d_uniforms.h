#ifndef GF3D_GF3D_UNIFORMS_H
#define GF3D_GF3D_UNIFORMS_H

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <linmath.h>

typedef struct
{
    mat4x4 model;
    mat4x4 view;
    mat4x4 proj;
} UniformBufferObject;

typedef struct
{
    uint32_t num_entities;
    uint32_t num_swap_chain_images;
    uint32_t num_uniform_buffer_objects;

    // Device allocation
    VkDeviceSize ubo_buffers_size;
    VkBuffer ubo_buffers_buffer;
    VkDeviceMemory ubo_buffers_device_memory;

    UniformBufferObject *current_ubo_states;
} gf3d_ubo_manager;

/**
 * Create a ubo manager.
 *
 * @param num_entities
 * @param num_swap_chain_images
 * @param render_height
 * @param render_width
 * @return
 */
gf3d_ubo_manager *gf3d_uniforms_init(uint32_t num_entities, uint32_t num_swap_chain_images, int render_width, int render_height);

void gf3d_uniforms_free(const gf3d_ubo_manager *self);

/**
 * Find the offset of an entity and frame's UBO from the buffer that backs it
 *
 * @param self
 * @param entity_id
 * @param swap_chain_frame_id
 * @return
 */
uint32_t gf3d_uniforms_reference_offset_get(gf3d_ubo_manager *self, uint32_t entity_id, uint32_t swap_chain_frame_id);

UniformBufferObject *gf3d_uniforms_reference_local_get(gf3d_ubo_manager *self, uint32_t entity_id);

uint32_t gf3d_uniforms_size();

/**
 * Flush the memory region for the GPU
 * @param self
 * @param swap_chain_image_id
 */
void gf3d_uniforms_flush(gf3d_ubo_manager *self, uint32_t swap_chain_image_id);

#endif
