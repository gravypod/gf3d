#include "gf3d_texture.h"
#include "gf3d_vgraphics.h"
#include "gf3d_swapchain.h"
#include "simple_logger.h"

#include <SDL_image.h>
#include <stdbool.h>


typedef struct
{
    Uint32          max_textures;
    Texture     *   texture_list;
    VkDevice        device;
}TextureManager;

static TextureManager gf3d_texture = {0};

void gf3d_texture_close();
void gf3d_texture_delete(Texture *tex);
void gf3d_texture_delete_all();

void gf3d_texture_init(Uint32 max_textures)
{
    slog("initializing texture system");
    if (!max_textures)
    {
        slog("cannot initialize texture system for 0 textures");
        return;
    }
    gf3d_texture.texture_list = gf3d_allocate_array(sizeof(Texture),max_textures);
    if (!gf3d_texture.texture_list)
    {
        slog("failed to initialize texture system: not enough memory");
        return;
    }
    gf3d_texture.max_textures = max_textures;
    gf3d_texture.device = gf3d_vgraphics_get_default_logical_device();
    atexit(gf3d_texture_close);
    slog("texture system initialized");
}

void gf3d_texture_close()
{
    slog("cleaning up textures");
    gf3d_texture_delete_all();
    if (gf3d_texture.texture_list != NULL)
    {
        free(gf3d_texture.texture_list);
    }
}

Texture *gf3d_texture_new()
{
    int i;
    for (i = 0; i < gf3d_texture.max_textures; i++)
    {
        if (!gf3d_texture.texture_list[i]._inuse)
        {
            gf3d_texture.texture_list[i]._inuse = 1;
            gf3d_texture.texture_list[i]._refcount = 1;
            return &gf3d_texture.texture_list[i];
        }
    }
    for (i = 0; i < gf3d_texture.max_textures; i++)
    {
        if (!gf3d_texture.texture_list[i]._refcount)
        {
            gf3d_texture_delete(&gf3d_texture.texture_list[i]);
            gf3d_texture.texture_list[i]._refcount = 1;
            gf3d_texture.texture_list[i]._inuse = 1;
            return &gf3d_texture.texture_list[i];
        }
    }
    slog("no free texture space");
    return NULL;
}

void gf3d_texture_delete(Texture *tex)
{
    if ((!tex)||(!tex->_inuse))return;

    if (tex->textureSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(gf3d_texture.device, tex->textureSampler, NULL);
    }
    if (tex->textureImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(gf3d_texture.device, tex->textureImageView, NULL);
    }
    if (tex->textureImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(gf3d_texture.device, tex->textureImage, NULL);
    }
    if (tex->textureImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf3d_texture.device, tex->textureImageMemory, NULL);
    }
    memset(tex,0,sizeof(Texture));
}

void gf3d_texture_free(Texture *tex)
{
    if ((!tex)||(!tex->_refcount))return;
    tex->_refcount--;
}

void gf3d_texture_delete_all()
{
    int i;
    for (i = 0; i < gf3d_texture.max_textures; i++)
    {
        gf3d_texture_delete(&gf3d_texture.texture_list[i]);
    }
}

Texture *gf3d_texture_get_by_filename(char * filename)
{
    int i;
    if (!filename)return NULL;
    for (i = 0; i < gf3d_texture.max_textures; i++)
    {
        if (!gf3d_texture.texture_list[i]._inuse)continue;
        if (gf3d_line_cmp(gf3d_texture.texture_list[i].filename,filename)==0)
        {
            return &gf3d_texture.texture_list[i];
        }
    }
    return NULL;
}

void gf3d_texture_copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer;
    Command * commandPool;
    VkBufferImageCopy region = {0};

    commandPool = gf3d_vgraphics_get_graphics_command_pool();
    commandBuffer = gf3d_command_begin_single_time(commandPool);


    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;

    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    gf3d_command_end_single_time(commandPool, commandBuffer);
}

void gf3d_texture_create_sampler(Texture *tex)
{
    VkSamplerCreateInfo samplerInfo = {0};

    if (!tex)return;

    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(gf3d_texture.device, &samplerInfo, NULL, &tex->textureSampler) != VK_SUCCESS)
    {
        slog("failed to create texture sampler!");
        return;
    }
    slog("created texture sampler");
}

void gf3d_texture_copy_staging_buffer_over_image_buffer(Texture *texture, bool already_copied_to)
{
    VkImageLayout current_layout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (already_copied_to) {
        current_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    gf3d_swapchain_transition_image_layout(
            texture->textureImage,
            VK_FORMAT_R8G8B8A8_UNORM,
            current_layout,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
    {
        gf3d_texture_copy_buffer_to_image(texture->stagingBuffer, texture->textureImage, texture->width, texture->height);
    }
    gf3d_swapchain_transition_image_layout(
            texture->textureImage,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
}

bool gf3d_texture_create_image(Texture *texture)
{
    static VkMemoryRequirements memoryRequirements;
    static VkMemoryAllocateInfo allocateInfo;
    static VkImageCreateInfo imageInfo;
    memset(&imageInfo, 0, sizeof(imageInfo));
    {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texture->width;
        imageInfo.extent.height = texture->height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
    }

    if (vkCreateImage(gf3d_texture.device, &imageInfo, NULL, &texture->textureImage) != VK_SUCCESS)
    {
        return false;
    }

    vkGetImageMemoryRequirements(gf3d_texture.device, texture->textureImage, &memoryRequirements);

    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = gf3d_vgraphics_find_memory_type(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(gf3d_texture.device, &allocateInfo, NULL, &texture->textureImageMemory) != VK_SUCCESS)
    {
        return false;
    }

    vkBindImageMemory(gf3d_texture.device, texture->textureImage, texture->textureImageMemory, 0);

    gf3d_texture_copy_staging_buffer_over_image_buffer(texture, false);

    texture->textureImageView = gf3d_vgraphics_create_image_view(texture->textureImage, VK_FORMAT_R8G8B8A8_UNORM);

    gf3d_texture_create_sampler(texture);

    return true;
}

void gf3d_texture_transfer_surface_to_buffer(Texture *texture, SDL_Surface *surface)
{
    SDL_LockSurface(surface);
        void* data;
        vkMapMemory(gf3d_texture.device, texture->stagingBufferMemory, 0, texture->texturePixelDataSize, 0, &data);
            memcpy(data, surface->pixels, texture->texturePixelDataSize);
        vkUnmapMemory(gf3d_texture.device, texture->stagingBufferMemory);
    SDL_UnlockSurface(surface);
}

void gf3d_texture_surface_update(Texture *texture, SDL_Surface *texture_surface)
{
    SDL_Surface *surface = texture_surface;

    // Make sure `surface` is the right pixel format
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    }

    // Copy texture data into staging buffer
    gf3d_texture_transfer_surface_to_buffer(texture, surface);
    gf3d_texture_copy_staging_buffer_over_image_buffer(texture, true);


    // Free the surface if there was a conversion
    if (surface != texture_surface) {
        SDL_FreeSurface(surface);
    }
}

Texture *gf3d_texture_surface_create(SDL_Surface *texture_surface)
{
    Texture *texture = gf3d_texture_new();
    gf3d_line_cpy(texture->filename, "placeholder");

    SDL_Surface *surface = texture_surface;

    // Make sure `surface` is the right pixel format
    if (surface->format->format != SDL_PIXELFORMAT_ABGR8888) {
        surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
    }

    // Find the size of the pixel data on the card
    texture->width = (uint32_t) surface->w;
    texture->height = (uint32_t) surface->h;
    texture->texturePixelDataSize = (VkDeviceSize) texture->width * texture->height * 4;

    // Create a staging buffer for texture data.
    gf3d_vgraphics_create_buffer(
            texture->texturePixelDataSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &texture->stagingBuffer, &texture->stagingBufferMemory
    );

    // Copy texture data into staging buffer
    gf3d_texture_transfer_surface_to_buffer(texture, surface);

    if (!gf3d_texture_create_image(texture)) {
        // Free the surface if there was a conversion
        if (surface != texture_surface) {
            SDL_FreeSurface(surface);
        }
        gf3d_texture_free(texture);
        return NULL;
    }

    // Free the surface if there was a conversion
    if (surface != texture_surface) {
        SDL_FreeSurface(surface);
    }

    return texture;
}

Texture *gf3d_texture_load(char *filename)
{
    Texture *tex = gf3d_texture_get_by_filename(filename);
    if (tex)
    {
        tex->_refcount++;
        return tex;
    }

    SDL_Surface *surface = IMG_Load(filename);
    if (!surface)
    {
        slog("failed to load texture file %s",filename);
        return NULL;
    }

    tex = gf3d_texture_surface_create(surface);

    if (!tex)
    {
        SDL_FreeSurface(surface);
        return NULL;
    }

    gf3d_line_cpy(tex->filename,filename);
    SDL_FreeSurface(surface);
    return tex;
}

/*eol@eof*/
