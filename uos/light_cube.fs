#version 440

uniform vec3 light_col;
uniform float light_pwr;

void main(){
	vec3 frag_col = light_pwr * light_col;
	gl_FragColor = vec4(frag_col, 1.f);
}