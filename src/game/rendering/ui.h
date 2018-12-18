#ifndef GF3D_RENDERNIG_UI_H
#define GF3D_RENDERNIG_UI_H

#include <gf3d_pipeline.h>
#include <gf3d_texture.h>

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

    Texture *interface_texture;

    rendering_pipeline_ui_screenspace_t screenspace;
} rendering_pipeline_ui_t;

/**
 * Create a rendering pipeline built for rendering flat 2D
 * user interfaces.
 *
 * @param interface_texture - Texture to render on the display
 * @param screen_width - Screen width of the window
 * @param screen_height - Screen height of the window
 * @return
 */
rendering_pipeline_ui_t *rendering_pipeline_ui_init(Texture *interface_texture, uint32_t screen_width, uint32_t screen_height);

/**
 * Bind, render, and unbind the UI pipeline.
 *
 * @param self - Pointer to the instance of the render pipeline
 * @param buffer - Command Buffer used for render pass
 * @param frame - Frame number we are on
 */
void rendering_pipeline_ui_renderpass(rendering_pipeline_ui_t *self, VkCommandBuffer buffer, Uint32 frame);


#endif
