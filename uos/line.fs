#version 440

in vec3 geo_col;

void main(){
	gl_FragColor = vec4(geo_col, 1.f);
}