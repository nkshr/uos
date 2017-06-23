#version 440
#pragma optimize(off)
#pragma debug(on)

in vec3 pos;
in vec3 col;

out vec3 vertex_pos;
out vec3 vertex_col;
void main(){
	vertex_col  =  col;
	vertex_pos = pos;
	gl_Position = vec4(vertex_pos, 1.f);
}