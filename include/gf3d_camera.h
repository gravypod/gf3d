#ifndef __GF3D_CAMERA_H__
#define __GF3D_CAMERA_H__

#include <linmath.h>
#include "gf3d_uniforms.h"


typedef struct
{
    GlobalUniformBufferObject *ubo;

    vec3 world_up;

    float width;
    float height;

    vec3 front, up, right;
    float *position, *rotation;

    float field_of_view;
    float near_plane;
    float far_plane;
} gf3d_camera;

void gf3d_camera_turn(gf3d_camera *self, float deltaPitch, float deltaYaw);

void gf3d_camera_move(gf3d_camera *self, float deltaForwardScale, float deltaRightScale, float deltaUpScale);

void gf3d_camera_update(gf3d_camera *self);

gf3d_camera *gf3d_camera_init(int render_width, int render_height, vec3 position, vec3 rotation);

#endif
