#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define NUM_BLOCKS 2.0f

// Terrible but only resource: https://www.khronos.org/opengl/wiki/Geometry_Shader
layout (points) in;
layout (invocations = 6) in;

layout (triangle_strip, max_vertices = 4) out; // 1 quad for each side of the qube


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
    const vec4 original_position = gl_in[instance].gl_Position;
    const float block_type = original_position.a;
    const float block_size = 1.0f;
    const vec3 block_offset = block_size * block_space_position;
    const vec4 modified_position = vec4(original_position.xyz + block_offset, 1.0f);

    textureCoord = vec2((texture_coord.x + (block_type - 1.0f)) / 2.0f,  texture_coord.y);
    gl_Position =   PV[instance] * modified_position;

    EmitVertex();
}

void quad(int instance, vec3 quadCoords[4], vec2 textureCoords[4])
{
	    point(instance, quadCoords[0], textureCoords[0]);
	    point(instance, quadCoords[1], textureCoords[1]);
	    point(instance, quadCoords[2], textureCoords[2]);
	    point(instance, quadCoords[3], textureCoords[3]);
    EndPrimitive();
}

vec3 positions[][] = {
    positions_top,
    positions_side_back,
    positions_side_front,
    positions_side_left,
    positions_side_right,
    positions_bottom
};

vec2 positions_textures[][] = {
    positions_texture_top,
    positions_texture_side_back,
    positions_texture_side_front,
    positions_texture_side_left,
    positions_texture_side_right,
    positions_texture_bottom
};



void main(void)
{

	for(int i = 0; i < gl_in.length(); i++)
	{
		quad(i, positions[gl_InvocationID], positions_textures[gl_InvocationID]);
    }
}