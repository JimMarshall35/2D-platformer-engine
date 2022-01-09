#version 460 core
layout (location = 0) in vec4 Vertex; // x,y == position, z,w == uv
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 model;

void main(){

	gl_Position = projection * model * vec4(Vertex.xy,0.0,1.0);
	TexCoords = Vertex.zw;
}