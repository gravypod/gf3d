#ifndef __GF3D_TEXTURE_H__
#define __GF3D_TEXTURE_H__

#include <vulkan/vulkan.h>
#include "gf3d_types.h"
#include "gf3d_text.h"

typedef struct
{
    Uint8               _inuse;
    Uint32              _refcount;
    TextLine            filename;
    VkImage             textureImage;
    VkDeviceMemory      textureImageMemory;
    VkImageView         textureImageView;
    VkSampler           textureSampler;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceSize        texturePixelDataSize;


    uint32_t width, height;
}Texture;


void gf3d_texture_init(Uint32 max_textures);

/**
 * Create a GF3D texture from a SDL surface.
 * @param texture_surface - Surface to use in the texture
 * @return Pointer to Texture or NULL if error
 */
Texture *gf3d_texture_surface_create(SDL_Surface *texture_surface);

/**
 * Update the texture data contained in the GPU for a given Texture object
 * @param texture - Pointer to a Texture to update
 * @param texture_surface - Pointer to an SDL_Surface to use as an image.
 */
void gf3d_texture_surface_update(Texture *texture, SDL_Surface *texture_surface);

Texture *gf3d_texture_load(char *filename);
void gf3d_texture_free(Texture *tex);

#endif
