#version 440
in vec3 vertex_col;
in vec3 vertex_normal_camera;
in vec3 vertex_light_dir_camera;
in vec3 vertex_pos_world;
in vec3 vertex_eye_dir_camera;
in vec2 vertex_st;
in float vertex_atten_coef;

uniform vec3 light_col;
uniform vec3 light_pos_world;
uniform vec3 amb_light_pwr;
uniform vec3 spec_light_col;
uniform float light_pwr;
uniform bool bamb;
uniform bool bdiffuse;
uniform bool bspec;
uniform bool batten;

float map_val(float max_val, float min_val, float mmax_val, float mmin_val, float val){
	return (val - min_val) * (mmax_val - mmin_val) / (max_val - min_val) + mmin_val;
}

void main(){
	vec3 e = normalize(vertex_eye_dir_camera);
	vec3 l = normalize(vertex_light_dir_camera);
	vec3 n = normalize(vertex_normal_camera);
	vec3 r = reflect(-l, n);

	float cos_theta = clamp(dot(n, l), 0.f, 1.f);
	float light_to_material_dist = length(light_pos_world -vertex_pos_world);
	float material_to_camera_dist = length(vertex_eye_dir_camera);
	float dist = light_to_material_dist + material_to_camera_dist;
	float atten;
	if(batten)
		atten = exp(-dist * vertex_atten_coef);
	else
		atten = 1.f;
	float cos_alpha = clamp(dot(r, e), 0.f, 1.f);
	vec3 amb_col = amb_light_pwr * vertex_col;
	
	vec3 frag_col = vec3(0);
	if(bamb)
		frag_col = amb_col;
	else
		frag_col = vec3(0.f);

	if(bdiffuse)
		frag_col += cos_theta * vertex_col * light_col * light_pwr * atten;

	if(bspec)
		frag_col += + spec_light_col * light_col * light_pwr * pow(cos_alpha, 5.f) * atten;
	gl_FragColor = vec4(frag_col, 1.f);
}