#include <linmath.h>
#include <string.h>
#include <gf3d_camera.h>
#include <gf3d_text.h>
#include <gf3d_uniforms.h>

GlobalUniformBufferObject *gf3d_camera_global_ubo;
int width, height;

vec3 eye = {2, 20, 2};
vec3 center = {0, 0, 0};
vec3 up = {0, 0, 1};


int gf3d_camera_width()
{
    return width;
}

int gf3d_camera_height()
{
    return height;
}

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
    mat4x4_translate(gf3d_camera_global_ubo->view, position[0], position[1], position[2]);
}

void gf3d_camera_rotate(vec3 rotation)
{
    vec3_add(eye, eye, rotation);
    mat4x4_look_at(
            gf3d_camera_global_ubo->view,
            eye,
            center,
            up
    );
}

void gf3d_camera_move(vec3 move)
{
    mat4x4_translate_in_place(gf3d_camera_global_ubo->view, move[0], move[1], move[2]);
}

void gf3d_camera_init(GlobalUniformBufferObject *global_ubo, int render_width, int render_height)
{
    gf3d_camera_global_ubo = global_ubo;

    mat4x4_identity(gf3d_camera_global_ubo->view);
    mat4x4_identity(gf3d_camera_global_ubo->proj);

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

    width = render_width;
    height = render_height;
}

/*eol@eof*/
