#version 440
in vec3 geo_color;
in vec3 geo_normal_camera;
in vec3 geo_light_dir_camera;
in vec3 geo_position_world;
in vec3 geo_eye_dir_camera;
in vec2 geo_st;

uniform vec3 light_color;
uniform vec3 light_position_world;
uniform float light_power;
uniform sampler2D tsampler;
uniform bool btexture;
uniform vec3 ambient_light_power;
uniform vec3 specular_color;
void main(){
	vec3 e = normalize(geo_eye_dir_camera);
	vec3 l = normalize(geo_light_dir_camera);
	vec3 n = normalize(geo_normal_camera);
	vec3 r = reflect(-n, n);

	float cos_theta = clamp(dot(n,l), 0.f, 1.f);
	float distance_lm = length(light_position_world - geo_position_world);

	vec3 ambient_color = ambient_light_power * geo_color;	
	float cos_alpha = clamp(dot(r, e), 0.f, 1.f);
	
	vec3 frag_color;

	float dd_lm = distance_lm * distance_lm;
	if(!btexture){
		frag_color = ambient_color + cos_theta * geo_color * light_color * light_power / (dd_lm)
		+ specular_color * light_color * light_power * pow(cos_alpha, 5)/ (dd_lm);
	}
	else{
		//gl_FragColor = texture(tsampler, geo_st);
	}

	gl_FragColor = vec4(frag_color, 1.f);
};