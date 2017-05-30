#version 440
in vec4 geo_color;
in vec3 geo_normal_camera;
in vec3 geo_light_dir_camera;
in vec3 geo_position_world;
in vec2 geo_st;

uniform vec4 light_color;
uniform vec3 light_position_world;
uniform float light_power;
uniform sampler2D tsampler;
uniform bool btexture;

void main(){
	float c = clamp(dot(geo_normal_camera, geo_light_dir_camera), 0.f, 1.f);
	float distance_lv = length(light_position_world - geo_position_world);
	if(!btexture){
		gl_FragColor = geo_color * light_color * light_power / (distance_lv * distance_lv) ;
	}
	else{
		gl_FragColor = texture(tsampler, geo_st);
	}
};