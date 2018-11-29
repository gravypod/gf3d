#include <gf3d_pipeline.h>
#include <gf3d_vgraphics.h>
#include <simple_logger.h>
#include <gf3d_swapchain.h>
#include "world.h"
#include "noise.h"
#include "entity/definitions/player.h"

/*typedef struct {
    vec3 position;
    float exists;
} block_ubo_t;*/

Pipeline *pipeline_world;


typedef struct {
    vec3 position;
} gpu_block;

typedef struct
{
    VkDeviceSize buffers_size;
    VkBuffer buffer;
    VkDeviceMemory memory;
} blockbuffer_gpu_t;


blockbuffer_gpu_t bound_block_buffer;


struct
{

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet *descriptorSets;
    VkDescriptorPool descriptorPool;
    Uint32 descriptorSetCount;
    uint32_t num_blocks;
    VkDeviceSize block_starting_pos;
} world_rendering;

//gf3d_ubo_manager *ubo_manager_world;
world_t world = {
        .seed = 10
};

/**
 * Generate a chunk
 *
 * @param c
 */
void world_chunk_generate(chunk_t *c)
{
    const long offset_x = CHUNK_TO_WORLD_X(c->major_x);
    const long offset_y = CHUNK_TO_WORLD_Y(c->major_y);

    for (long x = offset_x; x < offset_x + CHUNK_X; x++) {
        for (long y = offset_y; y < offset_y + CHUNK_Y; y++) {
            for (int z = 0; z < CHUNK_Z; z++) {
                long block_index = world_chunk_blocks_index(x, y, z);
                if (noise_block_exists(world.seed, x, y, z)) {
                    c->blocks[block_index] = 1;
                } else {
                    c->blocks[block_index] = 0;
                }
            }
        }
    }
}

void world_chunks_center()
{
    long player_chunk_x = WORLD_TO_CHUNK_X((long) player_entity->position[0]);
    long player_chunk_y = WORLD_TO_CHUNK_Y((long) player_entity->position[1]);

    for (long index_chunk_x = player_chunk_x - 1; index_chunk_x <= player_chunk_x + 1; index_chunk_x++) {
        for (long index_chunk_y = player_chunk_y - 1; index_chunk_y <= player_chunk_y + 1; index_chunk_y++) {

            chunk_t *c = &world.chunks[
                    ((index_chunk_x - player_chunk_x) + 1) +
                    (((index_chunk_y - player_chunk_y) + 1) * MAX_CHUNK_DISTANCE)
            ];

            // This is already the correct chunk. Skip
            if (c->major_x == index_chunk_x && c->major_y == index_chunk_y) {
                continue;
            }

            c->major_x = index_chunk_x;
            c->major_y = index_chunk_y;
            world_chunk_generate(c);
        }
    }
}

void world_buffer(gpu_block *world_block_views, uint32_t *num_blocks, VkDeviceSize *offset)
{
    uint32_t blocks = 0;
    uint32_t i = 0;
    for (int chunk_idx = 0; chunk_idx < MAX_NUM_LOADED_CHUNKS; chunk_idx++) {
        for (int block_idx = 0; block_idx < CHUNK_SIZE; block_idx++) {

            // No block is here
            if (world.chunks[chunk_idx].blocks[block_idx] == 0) {
                continue;
            }

            blocks++;

            gpu_block *block = &world_block_views[i++];
            long x, y, z;
            world_chunk_world_index(block_idx, &x, &y, &z);

            block->position[0] = x + world.chunks[chunk_idx].major_x;
            block->position[2] = y + world.chunks[chunk_idx].major_y;
            block->position[1] = z;
        }
    }
    *offset = blocks * sizeof(gpu_block);
    *num_blocks = blocks;
}

void world_render_block_flush(uint32_t *num_blocks, VkDeviceSize *offset)
{
    gpu_block *mapped_memory = NULL;

    vkMapMemory(gf3d_vgraphics_get_default_logical_device(), bound_block_buffer.memory, 0, bound_block_buffer.buffers_size, 0, (void *) &mapped_memory);
    {
        world_buffer(mapped_memory, num_blocks, offset);

        VkMappedMemoryRange memory_range = {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .size = bound_block_buffer.buffers_size,
                .offset = 0,
                .memory = bound_block_buffer.memory,
                .pNext = 0
        };
        vkFlushMappedMemoryRanges(gf3d_vgraphics_get_default_logical_device(), 1, &memory_range);
    }
    vkUnmapMemory(gf3d_vgraphics_get_default_logical_device(), bound_block_buffer.memory);
}

void world_update()
{
    world_chunks_center();
    world_render_block_flush(&world_rendering.num_blocks, &world_rendering.block_starting_pos);
}

void world_render(VkCommandBuffer buffer, Uint32 frame)
{
    const VkDescriptorSet *descriptorSet = &world_rendering.descriptorSets[frame];
    const uint32_t dynamic_offsets[1] = {
            gf3d_uniforms_reference_offset_get(gf3d_vgraphics_get_global_uniform_buffer_manager(), 0, frame)
    };
    VkDeviceSize block_start_position = 0;

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_world->pipeline);

    vkCmdBindDescriptorSets(
            buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_world->pipelineLayout,
            0,
            1, descriptorSet,
            1, dynamic_offsets
    );

    vkCmdBindVertexBuffers(buffer, 0, 1, &bound_block_buffer.buffer, &block_start_position);
    vkCmdDraw(buffer, world_rendering.num_blocks, 1, 0, 0);
}

void world_rendering_descriptor_set_pool_create()
{
    VkDescriptorPoolSize poolSize[1] = {0};
    VkDescriptorPoolCreateInfo poolInfo = {0};
    slog("attempting to make descriptor pools of size %i", gf3d_swapchain_get_swap_image_count());
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = gf3d_swapchain_get_swap_image_count();

    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = gf3d_swapchain_get_swap_image_count();

    if (vkCreateDescriptorPool(gf3d_vgraphics_get_default_logical_device(), &poolInfo, NULL, &world_rendering.descriptorPool) != VK_SUCCESS) {
        slog("failed to create descriptor pool!");
        return;
    }
}

void world_graphics_create_descriptor_sets()
{

    VkDescriptorSetLayout *layouts = NULL;
    VkDescriptorSetAllocateInfo allocInfo = {0};
    VkDescriptorBufferInfo global_ubo_info = {0};
    VkWriteDescriptorSet descriptorWrite[1] = {0};

    layouts = (VkDescriptorSetLayout *) gf3d_allocate_array(sizeof(VkDescriptorSetLayout), gf3d_swapchain_get_swap_image_count());
    for (int i = 0; i < gf3d_swapchain_get_swap_image_count(); i++) {
        memcpy(&layouts[i], &world_rendering.descriptorSetLayout, sizeof(VkDescriptorSetLayout));
    }

    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = world_rendering.descriptorPool;
    allocInfo.descriptorSetCount = gf3d_swapchain_get_swap_image_count();
    allocInfo.pSetLayouts = layouts;

    world_rendering.descriptorSets = (VkDescriptorSet *) gf3d_allocate_array(sizeof(VkDescriptorSet), gf3d_swapchain_get_swap_image_count());
    if (vkAllocateDescriptorSets(gf3d_vgraphics_get_default_logical_device(), &allocInfo, world_rendering.descriptorSets) != VK_SUCCESS) {
        slog("Failed to allocate descriptor sets");
        return;
    }
    world_rendering.descriptorSetCount = gf3d_swapchain_get_swap_image_count();
    for (int i = 0; i < gf3d_swapchain_get_swap_image_count(); i++) {

        global_ubo_info.buffer = gf3d_vgraphics_get_global_uniform_buffer_manager()->ubo_buffers_buffer;
        global_ubo_info.offset = 0;
        global_ubo_info.range = gf3d_vgraphics_get_global_uniform_buffer_manager()->size_ubo;

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = world_rendering.descriptorSets[i];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &global_ubo_info;
        descriptorWrite[0].pNext = NULL;

        vkUpdateDescriptorSets(gf3d_vgraphics_get_default_logical_device(), 1, descriptorWrite, 0, NULL);
    }
}

void world_graphics_descriptor_set_layout(VkDescriptorSetLayout *descriptorSetLayout)
{
    static VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    static VkDescriptorSetLayoutBinding bindings[1];
    memset(bindings, 0, sizeof(bindings));
/*

    VkDescriptorSetLayoutBinding *ubo_instance_binding = &bindings[0];
    {
        ubo_instance_binding->binding = 1;
        ubo_instance_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        ubo_instance_binding->descriptorCount = 1;
        ubo_instance_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_instance_binding->pImmutableSamplers = NULL; // Optional
    }
*/

    VkDescriptorSetLayoutBinding *ubo_global_binding = &bindings[0];
    {
        ubo_global_binding->binding = 0;
        ubo_global_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        ubo_global_binding->descriptorCount = 1;
        ubo_global_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_global_binding->pImmutableSamplers = NULL; // Optional
    }

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(gf3d_vgraphics_get_default_logical_device(), &layoutInfo, NULL, descriptorSetLayout) != VK_SUCCESS) {
        slog("failed to create descriptor set layout!");
    }
}


void world_graphics_pipeline_init()
{
    static VkVertexInputBindingDescription vertexInputBindingDescription;
    static VkVertexInputAttributeDescription vertexInputAttributeDescription;
    static VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    static VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

    slog("World Graphics: Clearing Data Structures... ");
    memset(&vertexInputBindingDescription, 0, sizeof(vertexInputBindingDescription));
    memset(&vertexInputAttributeDescription, 0, sizeof(vertexInputAttributeDescription));
    memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));
    memset(&inputAssemblyStateCreateInfo, 0, sizeof(inputAssemblyStateCreateInfo));

    slog("World Graphics: Configuring... VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO");
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    {
        // Vertex Input Binding Description
        {
            // Voxel point
            vertexInputBindingDescription.binding = 0;
            vertexInputBindingDescription.stride = sizeof(gpu_block);
            vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
            vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
        }

        // Vertex Input Attribute Description
        {
            // vec3 inPosition
            vertexInputAttributeDescription.offset = 0;
            vertexInputAttributeDescription.binding = 0;
            vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;

            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
            vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexInputAttributeDescription;
        }
    }

    slog("World Graphics: Configuring... VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO");
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    {
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    }

    world_graphics_descriptor_set_layout(&world_rendering.descriptorSetLayout);

    slog("World Graphics: Building Pipeline...");
    pipeline_world = gf3d_pipeline_graphics_load_preconfigured(
            gf3d_vgraphics_get_default_logical_device(),
            "shaders/voxel.vert.spv", "shaders/voxel.geom.spv", "shaders/voxel.frag.spv",
            gf3d_vgraphics_get_view_extent(),
            &vertexInputStateCreateInfo,
            &inputAssemblyStateCreateInfo,
            &world_rendering.descriptorSetLayout
    );

    if (!pipeline_world) {
        slog("World Graphics: FAILED TO BUILD PIPELINE");
    }
}

void world_graphics_block_buffer_init()
{

    bound_block_buffer.buffers_size = MAX_NUM_LOADED_BLOCKS * sizeof(gpu_block);
    gf3d_vgraphics_create_buffer(
            bound_block_buffer.buffers_size,

            // Memory information
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT /*| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/,

            // Buffer information
            &bound_block_buffer.buffer,
            &bound_block_buffer.memory
    );
}

void world_graphics_init()
{
    world_graphics_pipeline_init();

    world_rendering_descriptor_set_pool_create();
    world_graphics_create_descriptor_sets();
    world_graphics_block_buffer_init();

}

void world_init()
{
    world_graphics_init();
    world_chunks_center();
}