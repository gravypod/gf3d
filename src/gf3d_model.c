#include <gf3d_uniforms.h>
#include "gf3d_model.h"
#include "gf3d_commands.h"
#include "gf3d_vgraphics.h"
#include "simple_logger.h"
#include "gf3d_obj_load.h"

typedef struct
{
    Model               *   model_list;
    Uint32                  max_models;
    Uint32                  chain_length;   /**<length of swap chain*/
    VkDevice                device;
    VkDescriptorSetLayout   descriptorSetLayout;
}ModelManager;

static ModelManager gf3d_model = {0};

void gf3d_model_delete(Model *model);

void gf3d_model_create_descriptor_pool(Model *model);
void gf3d_model_create_descriptor_sets(Model *model);
void gf3d_model_create_descriptor_set_layout();
VkDescriptorSetLayout * gf3d_model_get_descriptor_set_layout();

void gf3d_model_manager_close()
{
    int i;
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        gf3d_model_delete(&gf3d_model.model_list[i]);
    }
    if (gf3d_model.model_list)
    {
        free(gf3d_model.model_list);
    }
    if (gf3d_model.descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(gf3d_model.device, gf3d_model.descriptorSetLayout, NULL);
    }
    memset(&gf3d_model,0,sizeof(ModelManager));
    slog("model manager closed");
}

void gf3d_model_manager_init(Uint32 max_models,Uint32 chain_length,VkDevice device)
{
    if (max_models == 0)
    {
        slog("cannot intilizat model manager for 0 models");
        return;
    }
    gf3d_model.chain_length = chain_length;
    gf3d_model.model_list = (Model *)gf3d_allocate_array(sizeof(Model),max_models);
    gf3d_model.max_models = max_models;
    gf3d_model.device = device;
    gf3d_model_create_descriptor_set_layout();
    slog("model manager initiliazed");
    atexit(gf3d_model_manager_close);
}

Model * gf3d_model_new()
{
    int i;
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        if (!gf3d_model.model_list[i]._inuse)
        {
            gf3d_model.model_list[i]._inuse = 1;
            gf3d_model.model_list[i]._refcount = 1;
            return &gf3d_model.model_list[i];
        }
    }
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        if (!gf3d_model.model_list[i]._refcount)
        {
            gf3d_model_delete(&gf3d_model.model_list[i]);
            gf3d_model.model_list[i]._inuse = 1;
            gf3d_model.model_list[i]._refcount = 1;
            return &gf3d_model.model_list[i];
        }
    }
    slog("unable to make a new model, out of space");
    return NULL;
}

Model *gf3d_model_get_by_filename(char *filename)
{
    int i;
    if (!filename)return NULL;
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        if (!gf3d_model.model_list[i]._inuse)continue;
        if (gf3d_line_cmp(filename,gf3d_model.model_list[i].filename)==0)
        {
            gf3d_model.model_list[i]._refcount++;
            return &gf3d_model.model_list[i];
        }
    }
    return NULL;
}

void gf3d_model_setup(Model *model)
{
    gf3d_model_create_descriptor_pool(model);
    gf3d_model_create_descriptor_sets(model);
}

Model * gf3d_model_load(char * filename)
{
    TextLine assetname;
    Model *model;
    model = gf3d_model_get_by_filename(filename);
    if (model)return model;
    model = gf3d_model_new();
    if (!model)return NULL;
    snprintf(assetname,GF3DLINELEN,"models/%s.obj",filename);
    model->mesh = gf3d_mesh_load(assetname);

    snprintf(assetname,GF3DLINELEN,"images/%s.png",filename);
    model->texture = gf3d_texture_load(assetname);
    
    gf3d_model_setup(model);
    return model;
}

void gf3d_model_free(Model *model)
{
    if ((!model)||(!model->_refcount))return;
    model->_refcount--;
}

void gf3d_model_delete(Model *model)
{
    if (!model)return;
    
    gf3d_mesh_free(model->mesh);
    gf3d_texture_free(model->texture);
    
    if (model->descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(gf3d_model.device, model->descriptorPool, NULL);
    }

}

void gf3d_model_draw(uint32_t entity_id, uint32_t swap_chain_image_id, Model *model,Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
    if (!model)
    {
        slog("cannot render a NULL model");
        return;
    }
    gf3d_mesh_render(entity_id, swap_chain_image_id, model->mesh,commandBuffer,&model->descriptorSets[bufferFrame]);
}

void gf3d_model_create_descriptor_sets(Model *model)
{
    int i;
    VkDescriptorSetLayout *layouts = NULL;
    VkDescriptorSetAllocateInfo allocInfo = {0};
    VkDescriptorBufferInfo instance_ubo_info = {0};
    VkDescriptorBufferInfo global_ubo_info = {0};
    VkWriteDescriptorSet descriptorWrite[3] = {0};
    VkDescriptorImageInfo imageInfo = {0};

    layouts = (VkDescriptorSetLayout *)gf3d_allocate_array(sizeof(VkDescriptorSetLayout),gf3d_model.chain_length);
    for (i = 0; i < gf3d_model.chain_length; i++)
    {
        memcpy(&layouts[i],gf3d_model_get_descriptor_set_layout(model),sizeof(VkDescriptorSetLayout));
    }
    
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = model->descriptorPool;
    allocInfo.descriptorSetCount = gf3d_model.chain_length;
    allocInfo.pSetLayouts = layouts;
    
    model->descriptorSets = (VkDescriptorSet *)gf3d_allocate_array(sizeof(VkDescriptorSet),gf3d_model.chain_length);
    if (vkAllocateDescriptorSets(gf3d_model.device, &allocInfo, model->descriptorSets) != VK_SUCCESS)
    {
        slog("failed to allocate descriptor sets!");
        return;
    }
    model->descriptorSetCount = gf3d_model.chain_length;
    for (i = 0; i < gf3d_model.chain_length; i++)
    {
        slog("updating descriptor sets");
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = model->texture->textureImageView;
        imageInfo.sampler = model->texture->textureSampler;
    
        instance_ubo_info.buffer = gf3d_vgraphics_get_instance_uniform_buffer_manager()->ubo_buffers_buffer;
        instance_ubo_info.offset = 0;
        instance_ubo_info.range = gf3d_vgraphics_get_instance_uniform_buffer_manager()->size_ubo;

        global_ubo_info.buffer = gf3d_vgraphics_get_global_uniform_buffer_manager()->ubo_buffers_buffer;
        global_ubo_info.offset = 0;
        global_ubo_info.range = gf3d_vgraphics_get_global_uniform_buffer_manager()->size_ubo;

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = model->descriptorSets[i];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &instance_ubo_info;
        descriptorWrite[0].pNext = NULL;

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].dstSet = model->descriptorSets[i];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pBufferInfo = &global_ubo_info;
        descriptorWrite[1].pNext = NULL;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].dstSet = model->descriptorSets[i];
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = &imageInfo;
        descriptorWrite[2].pTexelBufferView = NULL; // Optional
        descriptorWrite[2].pNext = NULL;

        vkUpdateDescriptorSets(gf3d_model.device, 3, descriptorWrite, 0, NULL);
    }
}

VkDescriptorSet * gf3d_model_get_descriptor_set_by_index(Model *model,Uint32 index)
{
    if (index >= model->descriptorSetCount)
    {
        slog("no descriptor set with index %i",index);
        return NULL;
    }
    return &model->descriptorSets[index];
}


void gf3d_model_create_descriptor_pool(Model *model)
{
    VkDescriptorPoolSize poolSize[3] = {0};
    VkDescriptorPoolCreateInfo poolInfo = {0};
    slog("attempting to make descriptor pools of size %i",gf3d_model.chain_length);
    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[0].descriptorCount = gf3d_model.chain_length;
    poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize[1].descriptorCount = gf3d_model.chain_length;
    poolSize[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize[2].descriptorCount = gf3d_model.chain_length;
    
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 3;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = gf3d_model.chain_length;
    
    if (vkCreateDescriptorPool(gf3d_model.device, &poolInfo, NULL, &model->descriptorPool) != VK_SUCCESS)
    {
        slog("failed to create descriptor pool!");
        return;
    }
}

void gf3d_model_create_descriptor_set_layout()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {0};
    
    VkDescriptorSetLayoutBinding bindings[3];
    
    samplerLayoutBinding.binding = 2;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = NULL;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    memcpy(&bindings[2],&samplerLayoutBinding,sizeof(VkDescriptorSetLayoutBinding));

    uboLayoutBinding.binding = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL; // Optional

    memcpy(&bindings[1],&uboLayoutBinding,sizeof(VkDescriptorSetLayoutBinding));

    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL; // Optional

    memcpy(&bindings[0],&uboLayoutBinding,sizeof(VkDescriptorSetLayoutBinding));

    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(gf3d_model.device, &layoutInfo, NULL, &gf3d_model.descriptorSetLayout) != VK_SUCCESS)
    {
        slog("failed to create descriptor set layout!");
    }
}

VkDescriptorSetLayout * gf3d_model_get_descriptor_set_layout()
{
    return &gf3d_model.descriptorSetLayout;
}

/*eol@eof*/
