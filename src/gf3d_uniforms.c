#include <stdint.h>
#include <gf3d_uniforms.h>
#include <vulkan/vulkan.h>
#include <gf3d_vgraphics.h>

struct
{
    VkDeviceSize size;
    VkDeviceMemory uniform_buffer_memory;
    VkBuffer uniform_buffer_space;
    UniformBufferObject *mapped_uniform_buffer_space;
    uint32_t num_swapchain_images, num_entities;
} Uniforms;


uint32_t gf3d_uniforms_buffer_object_size()
{
    return sizeof(UniformBufferObject);
}

VkBuffer gf3d_uniforms_buffer_get()
{
    return Uniforms.uniform_buffer_space;
}

uint32_t gf3d_uniforms_buffer_offset_index_get(uint32_t swapchain_image_number, uint32_t entity_number)
{
    const uint32_t index = entity_number + (swapchain_image_number * Uniforms.num_entities);
    return (index);
}

UniformBufferObject *gf3d_uniforms_buffers_reference_get(uint32_t swapchain_image_number, uint32_t entity_number)
{
    return &Uniforms.mapped_uniform_buffer_space[gf3d_uniforms_buffer_offset_index_get(swapchain_image_number, entity_number)];
}

void gf3d_uniforms_buffers_init(uint32_t num_ubos)
{
    Uniforms.size = gf3d_uniforms_buffer_object_size() * num_ubos;
    gf3d_vgraphics_create_buffer(
            Uniforms.size,
            // TODO: We don't want this COHERENT
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &Uniforms.uniform_buffer_space, &Uniforms.uniform_buffer_memory
    );

    vkMapMemory(gf3d_vgraphics_get_default_logical_device(), Uniforms.uniform_buffer_memory, 0, Uniforms.size, 0, (void **) &Uniforms.uniform_buffer_space);
}

void gf3d_uniforms_buffers_free()
{
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    vkDestroyBuffer(device, Uniforms.uniform_buffer_space, NULL);
    vkFreeMemory(device, Uniforms.uniform_buffer_memory, NULL);
}


void gf3d_uniforms_flush()
{
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkMappedMemoryRange range = {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .size = Uniforms.size,
            .memory = Uniforms.uniform_buffer_memory,
            .offset = 0,
            .pNext = NULL,
    };
    vkFlushMappedMemoryRanges(device, 1, &range);
}

void gf3d_uniforms_free()
{
    gf3d_uniforms_buffers_free();
}

void gf3d_uniforms_init(uint32_t num_swapchain_images, uint32_t num_entities)
{
    Uniforms.num_swapchain_images = num_swapchain_images;
    Uniforms.num_entities = num_entities;

    const uint32_t max_num_ubos = Uniforms.num_swapchain_images * Uniforms.num_entities;

    gf3d_uniforms_buffers_init(max_num_ubos);
}