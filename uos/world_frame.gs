#version

layout(lines, invocations = 1) in;
layout(line_strip, max_vertices = 2) out;

uniform float step;

void main(){
	vec3 unit_vec gl_Position = gl_in[0].gl_Position - gl_in[1].gl_Position;
	unit_vec = unit_vec / length(unit_vec);

}