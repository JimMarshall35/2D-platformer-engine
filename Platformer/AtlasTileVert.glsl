#version 460 core
layout (location = 0) in vec2 aPos; 
layout (location = 1) in int aUvArrayIndex;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;

uniform vec2[4] uvValues;

void main(){

	gl_Position = projection * model * vec4(aPos,0.0,1.0);
	TexCoords = uvValues[aUvArrayIndex];
}