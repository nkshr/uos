#version 440

uniform mat4 mvp;
uniform vec3 light_pos_world;
uniform col

in vec3 pos_model;

void main(){
	gl_Position = mvp * pos_model;
}