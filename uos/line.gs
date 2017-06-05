#version 440

layout(lines, invocations = 1) in;
layout(line_strip, max_vertices = 2) out;

in vec3 vertex_col[];
out vec3 geo_col;

void main(){
	for(int i = 0; i < 2; ++i){
		gl_Position = gl_in[i].gl_Position;
		geo_col = vertex_col[i];
		EmitVertex();
	}
	EndPrimitive();
}