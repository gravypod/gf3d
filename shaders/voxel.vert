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

layout(location = 0) in vec3 inPosition;

void main()
{
    mat4 identity = mat4(1.0f);

    gl_Position = gubo.proj * gubo.view * identity * vec4(inPosition, 1.0);
    //fragColor = vec4(1, 1, 1, 1.f);
}
