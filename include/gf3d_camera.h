#ifndef __GF3D_CAMERA_H__
#define __GF3D_CAMERA_H__

#include <linmath.h>
#include "gf3d_uniforms.h"

void gf3d_camera_init(GlobalUniformBufferObject *global_ubo, int render_width, int render_height);

/**
 * @brief set the camera properties based on position and direction that the camera should be looking
 * @param position the location for the camera
 * @param target the point the camera should be looking at
 * @param up the direction considered to be "up"
 */
void gf3d_camera_look_at(
    vec3 position,
    vec3 target,
    vec3 up
);

/**
 * @brief explicitely set the camera positon, holding all other parameters the same
 * @param position the new position for the camera
 */
void gf3d_camera_set_position(vec3 position);

/**
 * @brief move the camera relatively based on the vector provided
 * @param move the ammount to move the camera
 */
void gf3d_camera_move(vec3 move);

#endif
