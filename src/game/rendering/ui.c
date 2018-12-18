#include "ui.h"

#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <gf3d_vgraphics.h>
#include <gf3d_swapchain.h>
#include <simple_logger.h>
#include <gf3d_texture.h>
#include <gf3d_model.h>

Texture *ui_texture = NULL;

#define NUM_SCREEN_COORDINATES 6


vec2 screen_coordinates[NUM_SCREEN_COORDINATES] = {
        {/* positions */ /*-0.5f, -0.5f,*/ /* texcoords */ -0.1f, -0.1f},
        {/* positions */ /* 0.5f, -0.5f,*/ /* texcoords */ 0.1f, -0.1f},
        {/* positions */ /* 0.5f,  0.5f,*/ /* texcoords */ 0.1f, 0.1f},
        {/* positions */ /* 0.5f,  0.5f,*/ /* texcoords */ 0.1f, 0.1f},
        {/* positions */ /*-0.5f,  0.5f,*/ /* texcoords */ -0.1f, 0.1f},
        {/* positions */ /*-0.5f, -0.5f,*/ /* texcoords */ -0.1f, -0.1f},
};

void rendering_pipeline_ui_buffer_allocate(rendering_pipeline_ui_t *self)
{
    self->screenspace.size = NUM_SCREEN_COORDINATES * sizeof(vec2);
    gf3d_vgraphics_create_buffer(
            self->screenspace.size,

            // Memory information
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

            // Buffer information
            &self->screenspace.buffer,
            &self->screenspace.memory
    );
}

void rendering_pipeline_ui_buffer_init(rendering_pipeline_ui_t *self)
{
    rendering_pipeline_ui_buffer_allocate(self);
    void *mapped_memory = NULL;

    vkMapMemory(gf3d_vgraphics_get_default_logical_device(), self->screenspace.memory, 0, self->screenspace.size, 0, &mapped_memory);
    {

        memcpy(mapped_memory, screen_coordinates, self->screenspace.size);

        const VkMappedMemoryRange memory_range = {
                .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .size = self->screenspace.size, // TODO take limits into account. size,
                .offset = 0,
                .memory = self->screenspace.memory,
                .pNext = 0
        };
        vkFlushMappedMemoryRanges(gf3d_vgraphics_get_default_logical_device(), 1, &memory_range);
    }
    vkUnmapMemory(gf3d_vgraphics_get_default_logical_device(), self->screenspace.memory);
}



void rendering_pipeline_ui_renderpass(rendering_pipeline_ui_t *self, VkCommandBuffer buffer, Uint32 frame)
{
    const VkDescriptorSet *descriptorSet = &self->descriptorSets[frame];

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, self->pipeline->pipeline);

        vkCmdBindDescriptorSets(
                buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                self->pipeline->pipelineLayout,
                0,
                1, descriptorSet,
                0, NULL
        );

        vkCmdDraw(buffer, 4, 1, 0, 0);


    // Rebind old pipeline.
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gf3d_vgraphics_get_graphics_pipeline()->pipeline);
}

void rendering_pipeline_ui_descriptor_set_pool_init(rendering_pipeline_ui_t *self)
{

    VkDescriptorPoolSize poolSize[1] = {0};
    VkDescriptorPoolCreateInfo poolInfo = {0};
    slog("attempting to make descriptor pools of size %i", gf3d_swapchain_get_swap_image_count());

    poolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[0].descriptorCount = gf3d_swapchain_get_swap_image_count();

    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = gf3d_swapchain_get_swap_image_count();

    if (vkCreateDescriptorPool(gf3d_vgraphics_get_default_logical_device(), &poolInfo, NULL, &self->descriptorPool) != VK_SUCCESS) {
        slog("failed to create descriptor pool!");
        return;
    }
}

void rendering_pipeline_ui_descriptor_set_init(rendering_pipeline_ui_t *self)
{

    VkDescriptorSetLayout *layouts = NULL;
    VkDescriptorSetAllocateInfo allocInfo = {0};
    VkWriteDescriptorSet descriptorWrite[1] = {0};
    VkDescriptorImageInfo imageInfo = {0};

    layouts = (VkDescriptorSetLayout *) gf3d_allocate_array(sizeof(VkDescriptorSetLayout), gf3d_swapchain_get_swap_image_count());
    for (int i = 0; i < gf3d_swapchain_get_swap_image_count(); i++) {
        memcpy(&layouts[i], &self->descriptorSetLayout, sizeof(VkDescriptorSetLayout));
    }

    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = self->descriptorPool;
    allocInfo.descriptorSetCount = gf3d_swapchain_get_swap_image_count();
    allocInfo.pSetLayouts = layouts;

    self->descriptorSets = (VkDescriptorSet *) gf3d_allocate_array(sizeof(VkDescriptorSet), gf3d_swapchain_get_swap_image_count());
    if (vkAllocateDescriptorSets(gf3d_vgraphics_get_default_logical_device(), &allocInfo, self->descriptorSets) != VK_SUCCESS) {
        slog("Failed to allocate descriptor sets");
        return;
    }
    self->descriptorSetCount = gf3d_swapchain_get_swap_image_count();
    for (int i = 0; i < gf3d_swapchain_get_swap_image_count(); i++) {
        slog("updating descriptor sets");
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = ui_texture->textureImageView;
        imageInfo.sampler = ui_texture->textureSampler;

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = self->descriptorSets[i];
        descriptorWrite[0].dstBinding = 1;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pImageInfo = &imageInfo;
        descriptorWrite[0].pTexelBufferView = NULL; // Optional
        descriptorWrite[0].pNext = NULL;

        vkUpdateDescriptorSets(gf3d_vgraphics_get_default_logical_device(), 1, descriptorWrite, 0, NULL);
    }
}

void rendering_pipeline_ui_graphics_descriptor_set_layout(VkDescriptorSetLayout *descriptorSetLayout)
{
    static VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    static VkDescriptorSetLayoutBinding bindings[1];
    memset(bindings, 0, sizeof(bindings));

    VkDescriptorSetLayoutBinding *block_texture_binding = &bindings[0];
    {
        block_texture_binding->binding = 1;
        block_texture_binding->descriptorCount = 1;
        block_texture_binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        block_texture_binding->pImmutableSamplers = NULL;
        block_texture_binding->stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(gf3d_vgraphics_get_default_logical_device(), &layoutInfo, NULL, descriptorSetLayout) != VK_SUCCESS) {
        slog("failed to create descriptor set layout!");
    }
}



void rendering_pipeline_graphics_ui_init(rendering_pipeline_ui_t *self)
{
    static VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    static VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

    slog("UI Graphics: Clearing Data Structures... ");
    memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));
    memset(&inputAssemblyStateCreateInfo, 0, sizeof(inputAssemblyStateCreateInfo));

    slog("UI Graphics: Configuring... VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO");
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    {
        // Vertex Input Binding Description
        {
            // Voxel point
            vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
            vertexInputStateCreateInfo.pVertexBindingDescriptions = NULL;
        }

        // Vertex Input Attribute Description
        {

            vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
            vertexInputStateCreateInfo.pVertexAttributeDescriptions = NULL;
        }
    }

    slog("UI Graphics: Configuring... VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO");
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    {
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    }

    rendering_pipeline_ui_graphics_descriptor_set_layout(&self->descriptorSetLayout);

    slog("UI Graphics: Building Pipeline...");
    self->pipeline = gf3d_pipeline_graphics_load_preconfigured(
            gf3d_vgraphics_get_default_logical_device(),
            "shaders/ui.vert.spv", NULL, "shaders/ui.frag.spv",
            gf3d_vgraphics_get_view_extent(),
            &vertexInputStateCreateInfo,
            &inputAssemblyStateCreateInfo,
            &self->descriptorSetLayout
    );

    if (!self->pipeline) {
        slog("UI Graphics: FAILED TO BUILD PIPELINE");
    }
}


void rendering_pipeline_ui_update_vertex_data(rendering_pipeline_ui_t *self)
{
    for (size_t i = 0; i < NUM_SCREEN_COORDINATES; i++) {
        screen_coordinates[i][0] *= self->screen_width;
        screen_coordinates[i][1] *= self->screen_height;
    }
}

rendering_pipeline_ui_t *rendering_pipeline_ui_init(uint32_t screen_width, uint32_t screen_height)
{

    if (ui_texture == NULL) {
        ui_texture = gf3d_texture_load("./images/bs.png");
    }

    rendering_pipeline_ui_t *self = calloc(sizeof(rendering_pipeline_ui_t), 1);
    {
        self->screen_width = gf3d_vgraphics_get_view_extent().width;
        self->screen_height = gf3d_vgraphics_get_view_extent().height;

        //rendering_pipeline_ui_update_vertex_data(self)

        rendering_pipeline_ui_buffer_init(self);

        rendering_pipeline_graphics_ui_init(self);

        rendering_pipeline_ui_descriptor_set_pool_init(self);
        rendering_pipeline_ui_descriptor_set_init(self);
    }

    return self;
}
