#version 440

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 light_pos_world;

in vec3 col;
in vec3 pos_model;
in vec3 normal_model;
in vec2 st;

out vec3 vertex_col;
out vec3 vertex_normal_camera;
out vec3 vertex_light_dir_camera;
out vec3 vertex_pos_world;
out vec3 vertex_eye_dir_camera;
out vec2 vertex_st;

void main(){
	vertex_pos_world = (model * vec4(pos_model, 1.f)).xyz;
	vec4 pos_camera = view * vec4(vertex_pos_world, 1.f);
	gl_Position = proj * pos_camera;	
	vertex_col = col;
	vertex_normal_camera = (view * model * vec4(normal_model, 0)).xyz;
	vec3 light_pos_camera = vec3(view * vec4(light_pos_world, 1.f)).xyz;
	vertex_eye_dir_camera = vec3(0.f, 0.f, 0.f) - pos_camera.xyz;
	vertex_light_dir_camera = light_pos_camera + vertex_eye_dir_camera;
	vertex_st = st;
}