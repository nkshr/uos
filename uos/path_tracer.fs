#version 440
#pragma optimize(off)
#pragma debug(off)

in vec3 vertex_col;
void main(){
	gl_FragColor = vec4(vertex_col, 1.f);
}