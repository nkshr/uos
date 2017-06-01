#pragma once

class uos {
private:
	const char *vsname;
	const char *gsname;
	const char *fsname;

	shader_prog cube_sprog;
	shader_prog light_sprog;

	GLuint loc_pos;
	GLuint loc_col;
	GLuint loc_st;
	GLuint loc_nml;

	GLuint cube_vao;
	GLuint light_vao;

	Vector3f light_pos;
	Vector3f light_col;
	Vector3f ambient_light_col;

	float light_power;
	float ambient_light_power_r, ambient_light_power_g, ambient_light_power_b;
	float specular_r, specular_g, specular_b;

	vertices cube_face;

	Matrix4f cube_model;
	Matrix4f view;
	Matrix4f proj;

	void draw_cube_face();
	void destroy_cube_face();

public:
	uos();
	~uos();

	bool init();
	void destroy();

	bool prepare_shader_prog();
	void draw();


	void set_proj(const float *proj);
	void set_view(const float *view);
	void set_cube_model(const float *model);
	void set_light_pos(const  float x, const float y, const float z);
	void set_light_col(const float r, const float g, const float b);
	void set_light_pwr(const float pwr);
	void set_amb_light_pwr(const float r, const float g, const float b);
	void set_spec_col(const float r, const float g, const float b);
};