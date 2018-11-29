#include <gf3d_pipeline.h>
#include <gf3d_vgraphics.h>
#include <simple_logger.h>
#include <gf3d_swapchain.h>
#include "world.h"
#include "noise.h"
#include "entity/definitions/player.h"

typedef struct {
    vec3 position;
    float exists;
} block_ubo_t;

Pipeline *pipeline_world;
gf3d_ubo_manager *ubo_manager_world;
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

void world_buffer(world_block_view_t *world_block_views)
{
    size_t i = 0;
    for (int chunk_idx = 0; chunk_idx < MAX_NUM_LOADED_CHUNKS; chunk_idx++) {
        for (int block_idx = 0; block_idx < CHUNK_SIZE; block_idx++) {
            world_block_view_t *view = &world_block_views[i++];
            world_chunk_world_index(block_idx, view->x, view->y, view->z);
            *view->type = world.chunks[chunk_idx].blocks[block_idx];
        }
    }
}

void world_graphics_descriptor_set_layout(VkDescriptorSetLayout *descriptorSetLayout)
{
    static VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    static VkDescriptorSetLayoutBinding bindings[2];
    memset(bindings, 0, sizeof(bindings));

    VkDescriptorSetLayoutBinding *ubo_instance_binding = &bindings[0];
    {
        ubo_instance_binding->binding = 1;
        ubo_instance_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        ubo_instance_binding->descriptorCount = 1;
        ubo_instance_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_instance_binding->pImmutableSamplers = NULL; // Optional
    }

    VkDescriptorSetLayoutBinding *ubo_global_binding = &bindings[1];
    {
        ubo_global_binding->binding = 0;
        ubo_global_binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        ubo_global_binding->descriptorCount = 1;
        ubo_global_binding->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        ubo_global_binding->pImmutableSamplers = NULL; // Optional
    }

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(gf3d_vgraphics_get_default_logical_device(), &layoutInfo, NULL, descriptorSetLayout) != VK_SUCCESS)
    {
        slog("failed to create descriptor set layout!");
    }
}


void world_graphics_pipeline_init()
{
    static VkVertexInputBindingDescription vertexInputBindingDescription;
    static VkVertexInputAttributeDescription vertexInputAttributeDescription;
    static VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    static VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    static VkDescriptorSetLayout descriptorSetLayout;

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
            vertexInputBindingDescription.stride = sizeof(vec3);
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

    world_graphics_descriptor_set_layout(&descriptorSetLayout);

    slog("World Graphics: Building Pipeline...");
    pipeline_world = gf3d_pipeline_graphics_load_preconfigured(
            gf3d_vgraphics_get_default_logical_device(),
            "shaders/voxel.vert.spv", "shaders/voxel.geom.spv", "shaders/voxel.frag.spv",
            gf3d_vgraphics_get_view_extent(),
            &vertexInputStateCreateInfo,
            &inputAssemblyStateCreateInfo,
            &descriptorSetLayout
    );

    if (!pipeline_world) {
        slog("World Graphics: FAILED TO BUILD PIPELINE");
    }
}

void world_graphics_init()
{
    world_graphics_pipeline_init();
    ubo_manager_world = gf3d_uniforms_init(
            sizeof(block_ubo_t),
            MAX_NUM_LOADED_BLOCKS,
            gf3d_swapchain_get_swap_image_count()
    );
}

void world_init()
{
    world_graphics_init();
    world_chunks_center();
}