#version 440

uniform mat4 mvp;

in vec3 pos_model;
in vec3 col;
out vec3 vertex_col;

void main(){
	gl_Position = mvp * vec4(pos_model, 1.f);
	vertex_col = col;
}