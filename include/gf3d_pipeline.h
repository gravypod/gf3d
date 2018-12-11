#ifndef __GF3D_PIPELINE_H__
#define __GF3D_PIPELINE_H__

#include <vulkan/vulkan.h>

#include "gf3d_types.h"


typedef struct
{
    Bool                    inUse;
    VkPipeline              pipeline;
    VkRenderPass            renderPass;
    VkPipelineLayout        pipelineLayout;
    size_t                  vertSize;
    char                   *vertShader;
    VkShaderModule          vertModule;
    size_t                  geomSize;
    char                   *geomShader;
    VkShaderModule          geomModule;
    size_t                  fragSize;
    char                   *fragShader;
    VkShaderModule          fragModule;
    VkDevice                device;
}Pipeline;

/**
 * @brief setup pipeline system
 */
void gf3d_pipeline_init();

/**
 * @brief free a created pipeline
 */
void gf3d_pipeline_free(Pipeline *pipe);


/**
 * Load a pipeline configured to work with the model/mesh rendering code that DJ originally provided.
 * @param device - Provisioned device object
 * @param vertFile - Vertex shader to be used. If NULL it is omitted.
 * @param geomFile - Geometry shader to be used. If NULL it is omitted.
 * @param fragFile - Fragment shader to be used. If NULL it is omitted.
 * @param extent - Size of the screen to write to.
 * @param vertexInputInfo - The rate and layout of the vertex input data going to vertFile.
 * @param inputAssembly - Input layout for vertFile.
 * @param descriptorSetLayout - Descriptor sets to attach to this pipeline.
 * @return Pipeline* or NULL if failure. Check slog() output when this returns NULL for debug info.
 */
Pipeline *gf3d_pipeline_graphics_load_preconfigured(
        VkDevice device,
        char *vertFile, char *geomFile, char *fragFile,
        VkExtent2D extent,
        VkPipelineVertexInputStateCreateInfo *vertexInputInfo,
        VkPipelineInputAssemblyStateCreateInfo *inputAssembly,
        VkDescriptorSetLayout *descriptorSetLayout
);

/**
 * @brief setup a pipeline for rendering
 * @param device the logical device that the pipeline will be set up on
 * @param vertFile the filename of the vertex shader to use (expects spir-v byte code)
 * @param geomFile the filename of the geom shader to use (expects spir-v byte code)
 * @param fragFile the filename of the fragment shader to use (expects spir-v byte code)
 * @param extent the viewport dimensions for this pipeline
 * @returns NULL on error (see logs) or a pointer to a pipeline
 */
Pipeline *gf3d_pipeline_graphics_load(VkDevice device,char *vertFile,char *geomFile,char *fragFile,VkExtent2D extent);

VkFormat gf3d_pipeline_find_depth_format();

#endif
