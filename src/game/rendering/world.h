#ifndef GF3D_RENDERING_WORLD_H
#define GF3D_RENDERING_WORLD_H

#include <linmath.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <gf3d_pipeline.h>

/**
 * Block representation to be sent to the shader.
 */
typedef struct
{
    /**
     * X, Y, Z, Block Type
     */
    vec4 position;
} gpublock;

/**
 * Memory used for block data.
 */
typedef struct
{
    /**
     * Size on the card of block data.
     */
    VkDeviceSize buffers_size;

    /**
     * Buffer for block data.
     */
    VkBuffer buffer;

    /**
     * Memory backing the buffer.
     */
    VkDeviceMemory memory;
} rendering_pipeline_world_block_buffer;

typedef struct
{
    /**
     * Pipeline object
     */
    Pipeline *pipeline;
    /**
     * Descriptor layout for world rendering
     */
    VkDescriptorSetLayout descriptorSetLayout;

    /**
     * Descriptor data we push during render
     */
    VkDescriptorSet *descriptorSets;

    /**
     * Allocation pool for descriptor sets
     */
    VkDescriptorPool descriptorPool;

    /**
     * Number of descriptors we use. Currently one per frame
     */
    uint32_t descriptorSetCount;

    /**
     * Number of blocks we can render
     */
    uint32_t num_blocks;

    /**
     * Buffer of block data
     */
    rendering_pipeline_world_block_buffer block_buffer;
} rendering_pipeline_world;

/**
 * Create a render pipeline for the voxel world.
 *
 * @param num_blocks - Number of blocks we want to support at any given time.
 * @return
 */
rendering_pipeline_world *rendering_pipeline_world_init(uint32_t num_blocks);

/**
 * Send data about the voxel world to the GPU's shader memory location.
 *
 * @param self - Render Pipeline Configured for Voxels
 * @param blocks - Block data prepared for GPU.
 * @param num_blocks - Number of blocks we want to render.
 */
void rendering_pipeline_world_block_data_submit(rendering_pipeline_world *self, gpublock *blocks, size_t num_blocks);

/**
 * Run a render of the world space into a VkCommandBuffer.
 *
 * @param self - Render Pipeline Configured for Voxels
 * @param num_blocks - Number of blocks we want to render.
 * @param buffer - Command Buffer to Render Into.
 * @param frame - The frame id we are on.
 */
void rendering_pipeline_world_renderpass(rendering_pipeline_world *self, uint32_t num_blocks, VkCommandBuffer buffer, Uint32 frame);

#endif
