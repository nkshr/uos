#version 440

uniform mat4 mvp;

in vec3 pos_model;

void main(){
	gl_Position = mvp * vec4(pos_model, 1.f);
}