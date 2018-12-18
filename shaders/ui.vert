#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform GlobalUniformBufferObject {
    mat4 view;
    mat4 proj;
} gubo;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};

layout(location = 0) in vec4 inPosition;

void main()
{
    gl_Position = vec4(inPosition.y, inPosition.x, 0.f, 0.0f); // Scale position into 2d pixel space
}
