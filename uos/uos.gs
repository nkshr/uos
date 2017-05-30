#version 440
layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vertex_color[];
in vec3 vertex_normal_camera[];
in vec3 vertex_light_dir_camera[];
in vec3 vertex_position_world[];
in vec2 vertex_st[];

out vec4 geo_color;
out vec3 geo_normal_camera;
out vec3 geo_light_dir_camera;
out vec3 geo_position_world;
out vec2 geo_st;

void main(){
	for(int i = 0; i < 3; ++i){
		geo_color = vertex_color[i];
		geo_normal_camera = vertex_normal_camera[i];
		geo_light_dir_camera = vertex_light_dir_camera[i];
		geo_position_world = vertex_position_world[i];
		geo_st = vertex_st[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
};