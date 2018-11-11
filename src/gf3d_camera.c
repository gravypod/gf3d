#include <linmath.h>
#include <string.h>
#include <gf3d_camera.h>
#include <gf3d_text.h>
#include <gf3d_uniforms.h>

GlobalUniformBufferObject *gf3d_camera_global_ubo;


void gf3d_camera_look_at(
        vec3 position,
        vec3 target,
        vec3 up
)
{
    mat4x4_look_at(gf3d_camera_global_ubo->view, position, target, up);
}

void gf3d_camera_set_position(vec3 position)
{
    gf3d_camera_global_ubo->view[0][3] = position[0];
    gf3d_camera_global_ubo->view[1][3] = position[1];
    gf3d_camera_global_ubo->view[2][3] = position[2];
}

void gf3d_camera_move(vec3 move)
{
    gf3d_camera_global_ubo->view[0][3] += move[0];
    gf3d_camera_global_ubo->view[1][3] += move[1];
    gf3d_camera_global_ubo->view[2][3] += move[2];
}

void gf3d_camera_init(GlobalUniformBufferObject *global_ubo, int render_width, int render_height)
{
    gf3d_camera_global_ubo = global_ubo;

    mat4x4_identity(gf3d_camera_global_ubo->view);
    mat4x4_identity(gf3d_camera_global_ubo->proj);

    vec3 eye = {2, 20, 2};
    vec3 center = {0, 0, 0};
    vec3 up = {0, 0, 1};

    mat4x4_look_at(
            gf3d_camera_global_ubo->view,
            eye,
            center,
            up
    );
    mat4x4_perspective(
            gf3d_camera_global_ubo->proj,
            (float) (45 * GF3D_DEGTORAD),
            ((float) render_width) / ((float) render_height),
            0.1f,
            100
    );
    gf3d_camera_global_ubo->proj[1][1] *= -1;
}

/*eol@eof*/
