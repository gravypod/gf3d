#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Terrible but only resource: https://www.khronos.org/opengl/wiki/Geometry_Shader
layout (points) in;
layout (triangle_strip, max_vertices = 4*6) out; // 1 quad for each side of the qube

layout(location = 1) out vec2 textureCoord;
layout(location = 2) in mat4 PV[];


out gl_PerVertex {
    vec4 gl_Position;
};

//////////// Short hand for writing block-space models
#define BACK 1.0f
#define FRONT 0.0f

#define BOTTOM 0.0f
#define TOP 1.0f

#define LEFT 0.0f
#define RIGHT 1.0f

//////////// Short hand for writing texture-space positions
#define TOP_LEFT      0.f
#define TOP_RIGHT     1.f
#define TOP_TOP       0.f
#define TOP_BOTTOM    (1.f/3.f)

#define SIDE_LEFT     0.f
#define SIDE_RIGHT    1.f
#define SIDE_TOP      (2.f/3.f)
#define SIDE_BOTTOM   (3.f/3.f)

#define BOTTOM_LEFT   0.f
#define BOTTOM_RIGHT  1.f
#define BOTTOM_TOP    (1.f/3.f)
#define BOTTOM_BOTTOM (2.f/3.f)


// Block Top
vec3 positions_top[] = {
    vec3(FRONT,    TOP,  LEFT),
    vec3(FRONT,    TOP, RIGHT),
    vec3( BACK,    TOP,  LEFT),
    vec3( BACK,    TOP, RIGHT),
};
vec2 positions_texture_top[] = {
    vec2(    TOP_LEFT,       TOP_TOP),
    vec2(   TOP_RIGHT,       TOP_TOP),
    vec2(    TOP_LEFT,    TOP_BOTTOM),
    vec2(   TOP_RIGHT,    TOP_BOTTOM),
};

// Block Sides

/////////////////// Block Front
vec3 positions_side_front[] = {
    vec3(FRONT,    TOP,  LEFT),

    vec3(FRONT, BOTTOM,  LEFT),
    vec3(FRONT,    TOP, RIGHT),

    vec3(FRONT, BOTTOM, RIGHT),
};
vec2 positions_texture_side_front[] = {
    vec2(    SIDE_LEFT,     SIDE_TOP),

    vec2(    SIDE_LEFT,    SIDE_BOTTOM),
    vec2(   SIDE_RIGHT,      SIDE_TOP),

    vec2(   SIDE_RIGHT,    SIDE_BOTTOM),
};


/////////////////// Block Back
vec3 positions_side_back[] = {
    vec3( BACK,    TOP,  LEFT),
    vec3( BACK,    TOP, RIGHT),
    vec3( BACK, BOTTOM,  LEFT),
    vec3( BACK, BOTTOM, RIGHT),
};
vec2 positions_texture_side_back[] = {
    vec2(    SIDE_LEFT,     SIDE_TOP),
    vec2(   SIDE_RIGHT,      SIDE_TOP),
    vec2(    SIDE_LEFT,    SIDE_BOTTOM),
    vec2(   SIDE_RIGHT,    SIDE_BOTTOM),
};

/////////////////// Block Left
vec3 positions_side_left[] = {
    vec3( BACK,    TOP,  LEFT),
    vec3( BACK, BOTTOM,  LEFT),
    vec3(FRONT,    TOP,  LEFT),
    vec3(FRONT, BOTTOM,  LEFT),
};

vec2 positions_texture_side_left[] = {
    vec2(    SIDE_LEFT,     SIDE_TOP),
    vec2(    SIDE_LEFT,    SIDE_BOTTOM),
    vec2(   SIDE_RIGHT,      SIDE_TOP),
    vec2(   SIDE_RIGHT,    SIDE_BOTTOM),
};

/////////////////// Block Right
vec3 positions_side_right[] = {
    vec3( BACK,    TOP, RIGHT),
    vec3(FRONT,    TOP, RIGHT),
    vec3( BACK, BOTTOM, RIGHT),
    vec3(FRONT, BOTTOM, RIGHT),
};
vec2 positions_texture_side_right[] = {
    vec2(    SIDE_LEFT,     SIDE_TOP),
    vec2(   SIDE_RIGHT,      SIDE_TOP),
    vec2(    SIDE_LEFT,    SIDE_BOTTOM),
    vec2(   SIDE_RIGHT,    SIDE_BOTTOM),
};

// Block Bottom
vec3 positions_bottom[] = {
    vec3( BACK, BOTTOM,  LEFT),
    vec3( BACK, BOTTOM, RIGHT),
    vec3(FRONT, BOTTOM,  LEFT),
    vec3(FRONT, BOTTOM, RIGHT),
};
vec2 positions_texture_bottom[] = {
    vec2(    BOTTOM_LEFT,     BOTTOM_TOP),
    vec2(   BOTTOM_RIGHT,      BOTTOM_TOP),
    vec2(    BOTTOM_LEFT,    BOTTOM_BOTTOM),
    vec2(   BOTTOM_RIGHT,    BOTTOM_BOTTOM),
};


/**
 * Emit a point to the fragment shader. Attach all needed metadata and call EmitVertex()
 */
void point(int instance, vec3 block_space_position, vec2 texture_coord) {
    const float block_size = 1.f;
    textureCoord = texture_coord;
    gl_Position = PV[instance] * (gl_in[instance].gl_Position + vec4(block_size * block_space_position, 0.f));
    EmitVertex();
}

void quad(int instance, vec3 quadCoords[4], vec2 textureCoords[4])
{
    for (int i = 0; i < 4; i++)
	{
	    point(instance, quadCoords[i], textureCoords[i]);
    }
    EndPrimitive();
}

void main(void)
{

	for(int i = 0; i < gl_in.length(); i++)
	{
		quad(i, positions_top, positions_texture_top);

		quad(i, positions_side_back, positions_texture_side_back);
		quad(i, positions_side_front, positions_texture_side_front);

		quad(i, positions_side_left, positions_texture_side_left);
		quad(i, positions_side_right, positions_texture_side_right);

		quad(i, positions_bottom, positions_texture_bottom);
    }
}