#include <linmath.h>
#include <string.h>
#include <gf3d_camera.h>
#include <gf3d_text.h>
#include <gf3d_uniforms.h>
#include <gf3d_vgraphics.h>
#include <stdbool.h>

/**
 * Calculate the aspect ratio of the camera
 * @param self
 * @return
 */
static inline const float gf3d_camera_aspect_ratio(const gf3d_camera *const self)
{
    return self->width / self->height;
}

static inline const float gf3d_camera_pitch(const gf3d_camera *const self)
{
    return *self->rotation[0];
}

static inline const float gf3d_camera_yaw(const gf3d_camera *const self)
{
    return *self->rotation[1];
}


static inline void gf3d_camera_pitch_set(gf3d_camera *const self, float pitch)
{
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    *self->rotation[0] = pitch;
}

static inline void gf3d_camera_yaw_set(const gf3d_camera *const self, float yaw)
{
    yaw = fmodf(yaw, 360.0f);


    if (yaw < 0.0f)
        yaw += 360.0f;

    *self->rotation[1] = yaw;
}


void gf3d_camera_turn(gf3d_camera *const self, float deltaPitch, float deltaYaw)
{
    gf3d_camera_pitch_set(self, gf3d_camera_pitch(self) + deltaPitch);
    gf3d_camera_yaw_set(self, gf3d_camera_yaw(self) + deltaYaw);
}

void gf3d_camera_move(gf3d_camera *const self, float deltaForwardScale, float deltaRightScale, float deltaUpScale)
{
    vec3 deltaForward, deltaRight, deltaUp;

    vec3_scale(deltaForward, self->front, deltaForwardScale);
    vec3_scale(deltaRight, self->right, deltaRightScale);
    vec3_scale(deltaUp, self->up, deltaUpScale);

    vec3_add(*self->position, *self->position, deltaForward);
    vec3_add(*self->position, *self->position, deltaRight);
    vec3_add(*self->position, *self->position, deltaUp);

}

void gf3d_camera_vector_front_update(gf3d_camera *const self)
{
    double pitch = GF3D_DEG2RAD(gf3d_camera_pitch(self));
    double yaw = GF3D_DEG2RAD(gf3d_camera_yaw(self));
    self->front[0] = (float) (cos(yaw) * cos(pitch));
    self->front[1] = (float) sin(pitch);
    self->front[2] = (float) (sin(yaw) * cos(pitch));
    vec3_norm(self->front, self->front);
}

void gf3d_camera_vector_right_update(gf3d_camera *const self)
{
    vec3_mul_cross(self->right, self->front, self->world_up);
    vec3_norm(self->right, self->right);
}

void gf3d_camera_vector_up_update(gf3d_camera *const self)
{
    vec3_mul_cross(self->up, self->right, self->front);
    vec3_norm(self->up, self->up);
}


void gf3d_camera_update(gf3d_camera *const self)
{
    static vec3 target;

    // View matrix
    gf3d_camera_vector_front_update(self);
    gf3d_camera_vector_right_update(self);
    gf3d_camera_vector_up_update(self);

    vec3_add(target, *self->position, self->front);
    mat4x4_look_at(
            self->ubo->view,
            *self->position,
            target,
            self->up
    );

    // perspective matrix
    mat4x4_perspective(
            self->ubo->proj,
            self->field_of_view,
            gf3d_camera_aspect_ratio(self),
            self->near_plane, self->far_plane
    );
    self->ubo->proj[0][0] *= -1;
}

gf3d_camera *gf3d_camera_init(int render_width, int render_height, vec3 *position, vec3 *rotation)
{
    const vec3 defaultUp = {0, 1, 0};
    const vec3 defaultFront = {0.0f, 0.0f, -1.0f};

    gf3d_camera *self = calloc(sizeof(gf3d_camera), 1);

    self->ubo = gf3d_uniforms_reference_local_get(gf3d_vgraphics_get_global_uniform_buffer_manager(), 0);

    // Ups
    memcpy(self->up, defaultUp, sizeof(vec3));
    memcpy(self->world_up, defaultUp, sizeof(vec3));

    // Internal position structure
    self->position = position;
    self->rotation = rotation;

    // Front
    memcpy(self->front, defaultFront, sizeof(vec3));


    self->width = render_width;
    self->height = render_height;

    self->field_of_view = 50.0f;
    self->near_plane = 0.01f;
    self->far_plane = 100.0f;


    return self;
}
