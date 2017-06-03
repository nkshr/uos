#version 440
in vec3 vertex_col;
in vec3 vertex_normal_camera;
in vec3 vertex_light_dir_camera;
in vec3 vertex_pos_world;
in vec3 vertex_eye_dir_camera;
in vec2 vertex_st;

uniform vec3 light_col;
uniform vec3 light_pos_world;
uniform vec3 amb_light_pwr;
uniform vec3 spec_light_col;
uniform float light_pwr;

void main(){
	vec3 e = normalize(vertex_eye_dir_camera);
	vec3 l = normalize(vertex_light_dir_camera);
	vec3 n = normalize(vertex_normal_camera);
	vec3 r = reflect(-l, n);

	float cos_theta = clamp(dot(n, l), 0.f, 1.f);
	float light_to_material_dist = length(light_pos_world -vertex_pos_world);
	float lmdd = light_to_material_dist * light_to_material_dist;

	float cos_alpha = clamp(dot(r, e), 0.f, 1.f);
	vec3 amb_col = amb_light_pwr * vertex_col;
	//vec3 frag_col = amb_col+ cos_theta * vertex_col * light_col * light_pos_world / lmdd + spec_col * light_col * light_pwr * pow(cos_alpha, 5.f)/ lmdd;
	//vec3 frag_col = spec_light_col * light_col * light_pwr *  pow(cos_alpha, 5.f) / lmdd;
	vec3 frag_col = vertex_col;
	gl_FragColor = vec4(frag_col, 1.f);
	//gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);
}