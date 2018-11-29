#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


// Terrible but only resource: https://www.khronos.org/opengl/wiki/Geometry_Shader
layout (points) in;
layout (triangle_strip, max_vertices = 14) out;

layout(location = 2) in mat4 mProjection[];
layout(location = 6) in mat4 mView[];
layout(location = 10) in mat4 mModel[];


out gl_PerVertex {
    vec4 gl_Position;
};

float box_scale = 0.5f;
vec3 cube_strip_verts[] = {
    vec3(-box_scale, box_scale, box_scale),     // Front-top-left
    vec3(box_scale, box_scale, box_scale),      // Front-top-right
    vec3(-box_scale, -box_scale, box_scale),    // Front-bottom-left
    vec3(box_scale, -box_scale, box_scale),     // Front-bottom-right
    vec3(box_scale, -box_scale, -box_scale),    // Back-bottom-right
    vec3(box_scale, box_scale, box_scale),      // Front-top-right
    vec3(box_scale, box_scale, -box_scale),     // Back-top-right
    vec3(-box_scale, box_scale, box_scale),     // Front-top-left
    vec3(-box_scale, box_scale, -box_scale),    // Back-top-left
    vec3(-box_scale, -box_scale, box_scale),    // Front-bottom-left
    vec3(-box_scale, -box_scale, -box_scale),   // Back-bottom-left
    vec3(box_scale, -box_scale, -box_scale),    // Back-bottom-right
    vec3(-box_scale, box_scale, -box_scale),    // Back-top-left
    vec3(box_scale, box_scale, -box_scale)      // Back-top-right
};

void main(void)
{

	for(int i = 0; i < gl_in.length(); i++)
	{
	    for (int strip_idx = 0; strip_idx < 14; strip_idx++)
	    {

            gl_Position = mProjection[i] * mView[i] * /*mModel[i] **/ (gl_in[i].gl_Position + vec4(cube_strip_verts[strip_idx], 0.f));
            EmitVertex();
        }
        EndPrimitive();
    }
}