#ifndef GF3D_RENDERNIG_UI_H
#define GF3D_RENDERNIG_UI_H

#include <gf3d_pipeline.h>

/**
 * Memory used for block data.
 */
typedef struct
{
    VkDeviceSize size;
    VkBuffer buffer;
    VkDeviceMemory memory;
} rendering_pipeline_ui_screenspace_t;

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
     * Screen size
     */
    uint32_t screen_width, screen_height;

    rendering_pipeline_ui_screenspace_t screenspace;
} rendering_pipeline_ui_t;

rendering_pipeline_ui_t *rendering_pipeline_ui_init(uint32_t screen_width, uint32_t screen_height);

void rendering_pipeline_ui_renderpass(rendering_pipeline_ui_t *self, VkCommandBuffer buffer, Uint32 frame);


#endif
