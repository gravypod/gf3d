#version 450
#extension GL_ARB_separate_shader_objects : enable

#define NUM_UI_COORDS 4

vec2 positions[NUM_UI_COORDS] = vec2[](
    vec2(-1.0,  1.0), // Bottom Left
    vec2( 1.0,  1.0), // Bottom right
    vec2(-1.0, -1.0),  // Top left

    vec2( 1.0, -1.0)   // Top right
);

vec2 texture_positions[NUM_UI_COORDS] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),

    vec2(1.0, 0.0)
);

layout(location = 0) out vec2 texture_position;


void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f); // Scale position into 2d pixel space
    texture_position = texture_positions[gl_VertexIndex];
}
