#ifndef GF3D_GF3D_UNIFORMS_H
#define GF3D_GF3D_UNIFORMS_H

#include <vulkan/vulkan.h>
#include "gf3d_matrix.h"

typedef struct
{
    Matrix4 model;
    Matrix4 view;
    Matrix4 proj;
} UniformBufferObject;

/**
 * Obtain the size on the GPU of a UBO. This takes into account struct padding that has to happen
 * to make the data readable on the GPU/Driver side.
 *
 * TODO: Struct padding handling using VkDevicePhysicialLimits or something like that
 *
 * @return
 */
uint32_t gf3d_uniforms_buffer_object_size();

VkBuffer gf3d_uniforms_buffer_get();

uint32_t gf3d_uniforms_buffer_offset_index_get(uint32_t swapchain_image_number, uint32_t entity_number);
UniformBufferObject *gf3d_uniforms_buffers_reference_get(uint32_t swapchain_image_number, uint32_t entity_number);

void gf3d_uniforms_update(uint32_t num_swapchain_image, uint32_t num_entity, UniformBufferObject *ubo);

void gf3d_uniforms_free();
void gf3d_uniforms_init(uint32_t num_swapchain_images, uint32_t num_entities);

#endif
