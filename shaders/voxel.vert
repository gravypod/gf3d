#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform GlobalUniformBufferObject {
    mat4 view;
    mat4 proj;
} gubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec4 inPosition;

layout(location = 2) out mat4 PV;

void main()
{
    gl_Position = inPosition;
    PV = gubo.proj * gubo.view;
}
