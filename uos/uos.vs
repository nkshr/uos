#version 440
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 mv;
uniform mat4 mvp;
uniform vec3 light_position_world;

in vec4 color;
in vec3 position_model;
in vec3 normal_model;
in vec2 st;

out vec4 vertex_color;
out vec3 vertex_normal_camera;
out vec3 vertex_light_dir_camera;
out vec3 vertex_position_world;

out vec2 vertex_st;

void main(){
	vertex_position_world = (model * vec4(position_model, 1.f)).xyz;
	vec4 position_camera = mv * vec4(position_model, 1.f);
	gl_Position = proj * position_camera;
	vertex_color = color;
	vertex_normal_camera = normalize((view * model * vec4(normal_model, 1.f)).xyz);

	vec3 light_position_camera = (view * vec4(light_position_world, 1.f)).xyz;
	vec3 eye_dir_camera = vec3(0.f, 0.f, 0.f) - position_camera.xyz;
	vertex_light_dir_camera = normalize(light_position_camera + eye_dir_camera);

	vertex_st = st;
};