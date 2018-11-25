#ifndef __GF3D_MESH_H__
#define __GF3D_MESH_H__

#include <linmath.h>
#include <vulkan/vulkan.h>
#include "gf3d_text.h"

typedef struct
{
    vec3 vertex;
    vec3 normal;
    vec2 texel;
}Vertex;

typedef struct
{
    Uint32  verts[3];
}Face;

typedef struct {
    float x_min, x_max;
    float y_min, y_max;
    float z_min, z_max;
} mesh_dimenstions_t;

typedef struct
{
    TextLine        filename;
    Uint32          _refCount;
    Uint8           _inuse;
    Uint32          vertexCount;
    VkBuffer        buffer;
    VkDeviceMemory  bufferMemory;
    Uint32          faceCount;
    VkBuffer        faceBuffer;
    VkDeviceMemory  faceBufferMemory;
    mesh_dimenstions_t  dimensions;
}Mesh;

/**
 * @brief initializes the mesh system / configures internal data about mesh based rendering
 * @param mesh_max the maximum allowed simultaneous meshes supported at once.  Must be > 0
 */
void gf3d_mesh_init(Uint32 mesh_max);

/**
 * @brief load mesh data from the filename.
 * @note: currently only supporting obj files
 * @param filename the name of the file to load
 * @return NULL on error or Mesh data
 */
Mesh *gf3d_mesh_load(char *filename);

/**
 * @brief get the input attribute descriptions for mesh based rendering
 * @param count (optional, output) the number of attributes
 * @return a pointer to a vertex input attribute description array
 */
VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count);

/**
 * @brief get the binding description for mesh based rendering
 * @return vertex input binding descriptions compatible with mesh data
 */
VkVertexInputBindingDescription * gf3d_mesh_get_bind_description();

/**
 * @brief free a mesh that has been loaded from memory
 */
void gf3d_mesh_free(Mesh *mesh);

/**
 * @brief adds a mesh to the render pass
 * @note: must be called within the render pass
 * @param entity_id The ID of the entity you want to render
 * @param mesh The Swap Chain Image ID you want to render
 * @param com the command pool to use to handle the request we are rendering with
 */
void gf3d_mesh_render(uint32_t entity_id, uint32_t swap_chain_image_id, Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet);

/**
 * @brief create a mesh's internal buffers based on vertices
 * @param mesh the mesh handle to populate
 * @param vertices an array of vertices to make the mesh with
 * @param vcount how many vertices are in the array
 * @param faces an array of faces to make the mesh with
 * @param fcount how many faces are in the array
 */
void gf3d_mesh_create_vertex_buffer_from_vertices(Mesh *mesh,Vertex *vertices,Uint32 vcount,Face *faces,Uint32 fcount);

#endif
