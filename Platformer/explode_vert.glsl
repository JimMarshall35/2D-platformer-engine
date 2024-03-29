#version 460 core

#define QUAD_ROWS 16
#define QUAD_COLS 16
#define NUM_QUADS QUAD_COLS * QUAD_ROWS
#define TAU 6.2831853076


layout (location = 0) in vec2 position; 
layout (location = 1) in vec2 uv; 
layout (location = 2) in int quad_index; 

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;
uniform float time;
layout(binding = 0) uniform ParticleAttributesBlock
{
	vec2 directions[NUM_QUADS];
	float rotations[NUM_QUADS];
	float speeds[NUM_QUADS];
};
const float explodeAmount = 300;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(){
	//mat4 rotation_mat = rotationMatrix(vec3(0,0,1.0),rotations*time);
	vec4 worldpos = model * vec4(position,0.0,1.0);// *rotation_mat;
	worldpos.xy += mix(vec2(0.0),directions[quad_index]*speeds[quad_index],time);
	gl_Position = projection * worldpos;
	TexCoords = uv;
}