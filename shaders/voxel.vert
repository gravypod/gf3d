#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform block_ubo_t {
    vec3 position;
    float exists;
} ubo;

layout(binding = 1) uniform GlobalUniformBufferObject {
    mat4 view;
    mat4 proj;
} gubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec4 fragColor;

void main()
{
    gl_Position = gubo.proj * gubo.view * vec4(inPosition, 1.0);
    fragColor = vec4(0.5, 0.0, 0.0, ubo.exists);
}
