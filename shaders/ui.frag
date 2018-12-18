#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec2 texture_position;
layout(location = 0) out vec4 outColor;


void main()
{
    const vec4 color = texture(texSampler, texture_position);
    if (color.w < 1) {
        discard;
    }
    outColor = color;
}