#version 460 core
layout (location = 0) in vec2 pos; // x,y == position, z,w == uv

uniform mat4 projection;
uniform mat4 model;

void main(){

	gl_Position = projection * model * vec4(pos.xy,0.0,1.0);
}