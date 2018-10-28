#include <linmath.h>
#include <string.h>

mat4x4 gf3d_camera = {0};

void gf3d_camera_get_view(mat4x4 *view)
{
    if (!view)return;
    memcpy(view,gf3d_camera,sizeof(mat4x4 ));
}

void gf3d_camera_set_view(mat4x4 *view)
{
    if (!view)return;
    memcpy(gf3d_camera,view,sizeof(mat4x4 ));
}

void gf3d_camera_look_at(
    vec3 position,
    vec3 target,
    vec3 up
)
{
    mat4x4_look_at(gf3d_camera, position, target, up);
}

void gf3d_camera_set_position(vec3 position)
{
    gf3d_camera[0][3] = position[0];
    gf3d_camera[1][3] = position[1];
    gf3d_camera[2][3] = position[2];
}

void gf3d_camera_move(vec3 move)
{
    gf3d_camera[0][3] += move[0];
    gf3d_camera[1][3] += move[1];
    gf3d_camera[2][3] += move[2];
}

/*eol@eof*/
