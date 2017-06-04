#version 440

uniform mat4 vp;
uniform vec3 light_pos_world;

in vec3 pos_model;
in vec3 normal_model;


void main(){
	gl_Position = vp * vec4(pos_model + light_pos_world, 1.f);
}