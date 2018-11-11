#ifndef __GF3D_CAMERA_H__
#define __GF3D_CAMERA_H__

#include <linmath.h>
#include "gf3d_uniforms.h"

int gf3d_camera_width();

int gf3d_camera_height();


void gf3d_camera_init(GlobalUniformBufferObject *global_ubo, int render_width, int render_height);

/**
 * @brief explicitely set the camera positon, holding all other parameters the same
 * @param position the new position for the camera
 */
void gf3d_camera_set_position(vec3 position);

void gf3d_camera_rotate(vec3 rotation);

/**
 * @brief move the camera relatively based on the vector provided
 * @param move the ammount to move the camera
 */
void gf3d_camera_move(vec3 move);

#endif
