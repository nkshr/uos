#version 440

uniform vec3 light_col;

void main(){
	gl_FragColor = vec4(light_col, 1.f);
}