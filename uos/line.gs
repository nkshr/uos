#version 440
layout(lines, invocations = 1) in;
layout(line_strip, max_vertices = 10) out;)

uniform int width;
void main(){
	vec3 grad = 1.f / (gl_in[0].gl_Position - gl_in[1].gl_Position);
	for(int i = 0; i < width; ++i){
	
	}
}