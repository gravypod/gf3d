#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_vgraphics.h"
#include "gf3d_shaders.h"
#include "gf3d_model.h"
#include <string.h>
#include <stdio.h>
#include "simple_logger.h"

typedef struct
{
    Uint32      maxPipelines;
    Pipeline   *pipelineList;
}PipelineManager;

static PipelineManager gf3d_pipeline = {0};

void gf3d_pipeline_close();

void gf3d_pipeline_init(Uint32 max_pipelines)
{
    if (max_pipelines == 0)
    {
        slog("cannot initialize zero pipelines");
        return;
    }
    gf3d_pipeline.pipelineList = (Pipeline *)gf3d_allocate_array(sizeof(Pipeline),max_pipelines);
    if (!gf3d_pipeline.pipelineList)
    {
        slog("failed to allocate pipeline manager");
        return;
    }
    gf3d_pipeline.maxPipelines = max_pipelines;
    atexit(gf3d_pipeline_close);
    slog("pipeline system initialized");
}

void gf3d_pipeline_close()
{
    int i;
    slog("cleaning up pipelines");
    if (gf3d_pipeline.pipelineList != 0)
    {
        for (i = 0; i < gf3d_pipeline.maxPipelines; i++)
        {
            gf3d_pipeline_free(&gf3d_pipeline.pipelineList[i]);
        }
        free(gf3d_pipeline.pipelineList);
    }
    memset(&gf3d_pipeline,0,sizeof(PipelineManager));
}

Pipeline *gf3d_pipeline_new()
{
    int i;
    for (i = 0; i < gf3d_pipeline.maxPipelines; i++)
    {
        if (gf3d_pipeline.pipelineList[i].inUse)continue;
        gf3d_pipeline.pipelineList[i].inUse = true;
        return &gf3d_pipeline.pipelineList[i];
    }
    slog("no free pipelines");
    return NULL;
}

VkFormat gf3d_pipeline_find_supported_format(VkFormat * candidates, Uint32 candidateCount, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    int i;
    VkFormatProperties props = {0};
    for (i = 0; i < candidateCount;i++)
    {
        vkGetPhysicalDeviceFormatProperties(gf3d_vgraphics_get_default_physical_device(), candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return candidates[i];
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return candidates[i];
        }
    }

    slog("failed to find supported format!");
    return VK_NULL_HANDLE;
}

VkFormat gf3d_pipeline_find_depth_format()
{
    VkFormat formats[] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
    return gf3d_pipeline_find_supported_format(
        formats,3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void gf3d_pipeline_render_pass_setup(Pipeline *pipe)
{
    VkAttachmentDescription colorAttachment = {0};
    VkAttachmentReference colorAttachmentRef = {0};
    VkSubpassDescription subpass = {0};
    VkRenderPassCreateInfo renderPassInfo = {0};
    VkSubpassDependency dependency = {0};
    VkAttachmentDescription depthAttachment = {0};
    VkAttachmentReference depthAttachmentRef = {0};
    VkAttachmentDescription attachments[2];
    
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    depthAttachment.format = gf3d_pipeline_find_depth_format();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    colorAttachment.format = gf3d_swapchain_get_format();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    
    memcpy(&attachments[0],&colorAttachment,sizeof(VkAttachmentDescription));
    memcpy(&attachments[1],&depthAttachment,sizeof(VkAttachmentDescription));
    
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(pipe->device, &renderPassInfo, NULL, &pipe->renderPass) != VK_SUCCESS)
    {
        slog("failed to create render pass!");
        return;
    }
}


Pipeline *gf3d_pipeline_graphics_load_preconfigured(
        VkDevice device,
        char *vertFile, char *geomFile, char *fragFile,
        VkExtent2D extent,
        VkPipelineVertexInputStateCreateInfo *vertexInputInfo,
        VkPipelineInputAssemblyStateCreateInfo *inputAssembly,
        VkDescriptorSetLayout *descriptorSetLayout
)
{
    Pipeline *pipe;
    VkRect2D scissor = {0};
    VkViewport viewport = {0};
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    VkPipelineViewportStateCreateInfo viewportState = {0};
    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    VkPipelineShaderStageCreateInfo shaderStages[3];
    uint32_t num_shader_stages = 0;
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};

    pipe = gf3d_pipeline_new();
    if (!pipe)return NULL;

    {
        // Load shaders
        if (vertFile) {
            pipe->vertShader = gf3d_shaders_load_data(vertFile, &pipe->vertSize);
            pipe->vertModule = gf3d_shaders_create_module(pipe->vertShader, pipe->vertSize, device);
        }

        if (geomFile) {
            pipe->geomShader = gf3d_shaders_load_data(geomFile, &pipe->geomSize);
            pipe->geomModule = gf3d_shaders_create_module(pipe->geomShader, pipe->geomSize, device);
        }

        if (fragFile) {
            pipe->fragShader = gf3d_shaders_load_data(fragFile, &pipe->fragSize);
            pipe->fragModule = gf3d_shaders_create_module(pipe->fragShader, pipe->fragSize, device);
        }
    }

    pipe->device = device;

    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_TRUE;

    {
        memset(shaderStages, 0, sizeof(shaderStages));

        // Load up shaders based on arguments
        if (vertFile) {
            VkPipelineShaderStageCreateInfo *info = &shaderStages[num_shader_stages++];
            info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info->stage = VK_SHADER_STAGE_VERTEX_BIT;
            info->module = pipe->vertModule;
            info->pName = "main";
        }

        if (geomFile) {
            VkPipelineShaderStageCreateInfo *info = &shaderStages[num_shader_stages++];
            info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info->stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            info->module = pipe->geomModule;
            info->pName = "main";
        }

        if (fragFile) {
            VkPipelineShaderStageCreateInfo *info = &shaderStages[num_shader_stages++];
            info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            info->module = pipe->fragModule;
            info->pName = "main";
        }
    }

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = extent;

    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    /**
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
			pipelineRasterizationStateCreateInfo.cullMode = cullMode;
			pipelineRasterizationStateCreateInfo.frontFace = frontFace;
			pipelineRasterizationStateCreateInfo.flags = flags;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

     */
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE;

    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    gf3d_pipeline_render_pass_setup(pipe);

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipe->pipelineLayout) != VK_SUCCESS)
    {
        slog("failed to create pipeline layout!");
        gf3d_pipeline_free(pipe);
        return NULL;
    }

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = num_shader_stages;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = vertexInputInfo;
    pipelineInfo.pInputAssemblyState = inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = NULL; // Optional
    pipelineInfo.layout = pipe->pipelineLayout;
    pipelineInfo.renderPass = pipe->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    pipelineInfo.pDepthStencilState = &depthStencil;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &pipe->pipeline) != VK_SUCCESS)
    {
        slog("failed to create graphics pipeline!");
        gf3d_pipeline_free(pipe);
        return NULL;
    }
    return pipe;
}

Pipeline *gf3d_pipeline_graphics_load(VkDevice device, char *vertFile, char *geomFile, char *fragFile, VkExtent2D extent)
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = gf3d_mesh_get_bind_description(); // Optional
    vertexInputInfo.pVertexAttributeDescriptions = gf3d_mesh_get_attribute_descriptions(&vertexInputInfo.vertexAttributeDescriptionCount); // Optional    

    // TODO: pull all this information from config file
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    return gf3d_pipeline_graphics_load_preconfigured(
            device,
            vertFile, geomFile, fragFile,
            extent,
            &vertexInputInfo, &inputAssembly, gf3d_model_get_descriptor_set_layout()
    );
}

void gf3d_pipeline_free(Pipeline *pipe)
{
    if (!pipe)return;
    if (!pipe->inUse)return;
    if (pipe->pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(pipe->device, pipe->pipeline, NULL);
    }
    if (pipe->pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(pipe->device, pipe->pipelineLayout, NULL);
    }
    if (pipe->renderPass)
    {
        vkDestroyRenderPass(pipe->device, pipe->renderPass, NULL);
    }
    if (pipe->fragModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(pipe->device, pipe->fragModule, NULL);
    }
    if (pipe->vertModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(pipe->device, pipe->vertModule, NULL);
    }
    if (pipe->fragShader != NULL)
    {
        free(pipe->fragShader);
    }
    if (pipe->vertShader != NULL)
    {
        free (pipe->vertShader);
    }
    memset(pipe,0,sizeof(Pipeline));
}

/*eol@eof*/
