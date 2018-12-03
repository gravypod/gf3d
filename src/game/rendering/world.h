#ifndef GF3D_RENDERING_WORLD_H
#define GF3D_RENDERING_WORLD_H

#include <linmath.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <gf3d_pipeline.h>

typedef struct
{
    vec3 position;
} gpublock;

typedef struct
{
    VkDeviceSize buffers_size;
    VkBuffer buffer;
    VkDeviceMemory memory;
} rendering_pipeline_world_block_buffer;

typedef struct
{
    Pipeline *pipeline;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet *descriptorSets;
    VkDescriptorPool descriptorPool;
    uint32_t descriptorSetCount;
    uint32_t num_blocks;

    rendering_pipeline_world_block_buffer block_buffer;
} rendering_pipeline_world;

rendering_pipeline_world *rendering_pipeline_world_init(uint32_t num_blocks);


void rendering_pipeline_world_block_data_submit(rendering_pipeline_world *self, gpublock *blocks, size_t num_blocks);
void rendering_pipeline_world_renderpass(rendering_pipeline_world *self, uint32_t num_blocks, VkCommandBuffer buffer, Uint32 frame);

#endif
