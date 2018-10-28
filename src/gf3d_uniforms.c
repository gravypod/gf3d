#include <gf3d_vgraphics.h>
#include "gf3d_uniforms.h"

uint32_t gf3d_uniforms_size()
{
    return sizeof(UniformBufferObject);
}

/**
 * @return Size of UBO on the GPU card
 */
uint32_t gf3d_uniforms_aligned_size()
{
    static VkPhysicalDevice looked_up_device = VK_NULL_HANDLE;
    static uint32_t aligned_uniform_size = 0;

    VkPhysicalDevice current = gf3d_vgraphics_get_default_physical_device();

    if (current != looked_up_device) {
        looked_up_device = current;
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(current, &properties);
        uint32_t min_alignment = (uint32_t) properties.limits.minUniformBufferOffsetAlignment;

        aligned_uniform_size = gf3d_uniforms_size();

        // Check to see if we need to pad the UBO struct and add in the padding
        if (aligned_uniform_size % min_alignment > 0) {
            aligned_uniform_size += min_alignment - (aligned_uniform_size % min_alignment);
        }
    }

    return aligned_uniform_size;
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
    return gf3d_uniforms_reference_index_get(self, entity_id, swap_chain_frame_id) * gf3d_uniforms_aligned_size();
}

UniformBufferObject *gf3d_uniforms_reference_local_get(gf3d_ubo_manager *self, uint32_t entity_id)
{
    return ((void*)self->current_ubo_states) + (entity_id * gf3d_uniforms_aligned_size());
}

/**
 * Allocate uniform buffer memory on GPU for a UBO manager
 * @param self
 */
void gf3d_uniforms_buffer_allocate(gf3d_ubo_manager *const self)
{
    // Calculate buffer size;
    self->ubo_buffers_size = self->num_uniform_buffer_objects * gf3d_uniforms_aligned_size();
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

void gf3d_uniforms_buffer_objects_init(gf3d_ubo_manager *const self, const float aspect_ratio)
{
    static UniformBufferObject local;

    // Prepare the initial state of the UBO
    mat4x4_identity(local.model);
    mat4x4_identity(local.view);
    mat4x4_identity(local.proj);

    mat4x4_identity(local.view);

    vec3 eye = {2, 20, 2};
    vec3 center = {0, 0, 0};
    vec3 up = {0, 0, 1};
    mat4x4_look_at(
            local.view,
            eye,
            center,
            up
    );
    mat4x4_perspective(local.proj, (float) (45 * GF3D_DEGTORAD), aspect_ratio, 0.1f, 100);

    // Vulkan has a Y index is flipped from OpenGL
    local.proj[1][1] *= -1;

    for (uint32_t i = 0; i < self->num_entities; i++) {
        UniformBufferObject *const ubo = gf3d_uniforms_reference_local_get(self, i);
        memcpy(ubo, &local, sizeof(UniformBufferObject));
    }
}

void gf3d_uniforms_flush(gf3d_ubo_manager *self, uint32_t swap_chain_image_id)
{
    uint32_t ubo_swap_chain_image_beginning_offset = gf3d_uniforms_reference_offset_get(self, 0, swap_chain_image_id);
    uint32_t ubo_block_size = gf3d_uniforms_aligned_size() * self->num_entities;

    void *mapped_memory = NULL;

    vkMapMemory(
            gf3d_vgraphics_get_default_logical_device(),
            self->ubo_buffers_device_memory,
            ubo_swap_chain_image_beginning_offset, ubo_block_size,
            0,
            &mapped_memory
    );
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

gf3d_ubo_manager *gf3d_uniforms_init(uint32_t num_entities, uint32_t num_swap_chain_images, int render_width, int render_height)
{
    gf3d_ubo_manager *const self = calloc(sizeof(gf3d_ubo_manager), 1);

    self->num_entities = num_entities;
    self->num_swap_chain_images = num_swap_chain_images;
    self->num_uniform_buffer_objects = self->num_entities * self->num_swap_chain_images;

    self->current_ubo_states = calloc(gf3d_uniforms_aligned_size(), self->num_entities);

    gf3d_uniforms_buffer_allocate(self);
    gf3d_uniforms_buffer_objects_init(self, ((float) render_width) / ((float) render_height));

    return self;
}

void gf3d_uniforms_free(const gf3d_ubo_manager *const self)
{
    VkDevice logical_device = gf3d_vgraphics_get_default_logical_device();

    // Clear up the buffer
    vkDestroyBuffer(logical_device, self->ubo_buffers_buffer, NULL);
    vkFreeMemory(logical_device, self->ubo_buffers_device_memory, NULL);
}