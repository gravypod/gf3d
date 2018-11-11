#include <gf3d_vgraphics.h>
#include "gf3d_uniforms.h"

/**
 * @return Size of UBO on the GPU card
 */
uint32_t gf3d_uniforms_aligned_size(uint32_t size_ubo)
{
    VkPhysicalDevice current = gf3d_vgraphics_get_default_physical_device();

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(current, &properties);
    uint32_t min_alignment = (uint32_t) properties.limits.minUniformBufferOffsetAlignment;


    // Check to see if we need to pad the UBO struct and add in the padding
    if (size_ubo % min_alignment > 0) {
        return size_ubo + (min_alignment - (size_ubo % min_alignment));
    }

    return size_ubo;
}

/**
 *
 * Entities appear in the following format. The numbers below represent an index. E<num>
 * represents the entity ID.
 *
 * | 0 - E1 | 1 - E2 | 2 - E3 | (swap chain image 1)
 * | 3 - E1 | 4 - E2 | 5 - E3 | (swap chain image 2)
 *
 * @param self
 * @param entity_id
 * @param swap_chain_frame_id
 * @return
 */
uint32_t gf3d_uniforms_reference_index_get(gf3d_ubo_manager *self, uint32_t entity_id, uint32_t swap_chain_frame_id)
{
    return entity_id + (self->num_entities * swap_chain_frame_id);
}

uint32_t gf3d_uniforms_reference_offset_get(gf3d_ubo_manager *self, uint32_t entity_id, uint32_t swap_chain_frame_id)
{
    return gf3d_uniforms_reference_index_get(self, entity_id, swap_chain_frame_id) * self->size_aligned_ubo;
}

void *gf3d_uniforms_reference_local_get(gf3d_ubo_manager *self, uint32_t entity_id)
{
    return (self->current_ubo_states) + (entity_id * self->size_aligned_ubo);
}

/**
 * Allocate uniform buffer memory on GPU for a UBO manager
 * @param self
 */
void gf3d_uniforms_buffer_allocate(gf3d_ubo_manager *const self)
{
    // Calculate buffer size;
    self->ubo_buffers_size = self->num_uniform_buffer_objects * self->size_aligned_ubo;
    gf3d_vgraphics_create_buffer(
            self->ubo_buffers_size,

            // Memory information
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,

            // Buffer information
            &self->ubo_buffers_buffer,
            &self->ubo_buffers_device_memory
    );
}

void gf3d_uniforms_flush(gf3d_ubo_manager *self, uint32_t swap_chain_image_id)
{
    uint32_t ubo_swap_chain_image_beginning_offset = gf3d_uniforms_reference_offset_get(self, 0, swap_chain_image_id);
    uint32_t ubo_block_size = self->size_aligned_ubo * self->num_entities;

    void *mapped_memory = NULL;

    vkMapMemory(gf3d_vgraphics_get_default_logical_device(), self->ubo_buffers_device_memory, ubo_swap_chain_image_beginning_offset, ubo_block_size, 0, &mapped_memory);
    {
        memcpy(mapped_memory, self->current_ubo_states, ubo_block_size);
        // Flush to make changes visible to the host
        VkMappedMemoryRange memory_range = {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .size = ubo_block_size,
                .offset = ubo_swap_chain_image_beginning_offset,
                .memory = self->ubo_buffers_device_memory,
                .pNext = 0
        };
        vkFlushMappedMemoryRanges(gf3d_vgraphics_get_default_logical_device(), 1, &memory_range);
    }
    vkUnmapMemory(gf3d_vgraphics_get_default_logical_device(), self->ubo_buffers_device_memory);
}

gf3d_ubo_manager *gf3d_uniforms_init(uint32_t size_ubo, uint32_t num_entities, uint32_t num_swap_chain_images)
{
    gf3d_ubo_manager *const self = calloc(sizeof(gf3d_ubo_manager), 1);

    self->size_ubo = size_ubo;
    self->size_aligned_ubo = gf3d_uniforms_aligned_size(self->size_ubo);

    self->num_entities = num_entities;
    self->num_swap_chain_images = num_swap_chain_images;
    self->num_uniform_buffer_objects = self->num_entities * self->num_swap_chain_images;

    self->current_ubo_states = calloc(self->size_aligned_ubo, self->num_entities);

    gf3d_uniforms_buffer_allocate(self);

    return self;
}

void gf3d_uniforms_free(const gf3d_ubo_manager *const self)
{
    VkDevice logical_device = gf3d_vgraphics_get_default_logical_device();

    // Clear up the buffer
    vkDestroyBuffer(logical_device, self->ubo_buffers_buffer, NULL);
    vkFreeMemory(logical_device, self->ubo_buffers_device_memory, NULL);
}