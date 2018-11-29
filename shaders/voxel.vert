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

layout(location = 2) out mat4 mProjection;
layout(location = 6) out mat4 mView;
layout(location = 10) out mat4 mModel;

void main()
{
    mat4 identity = mat4(1.0f);

    gl_Position = /*gubo.proj * gubo.view * identity * */vec4(inPosition, 1.0);
    mProjection = gubo.proj;
    mView = gubo.view;
    mModel = identity;
    //fragColor = vec4(1, 1, 1, 1.f);
}
