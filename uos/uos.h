#pragma once

class uos {
private:
	const char *cube_face_vsname;
	const char *cube_face_fsname;
	const char *light_cube_vsname;
	const char *light_cube_fsname;

	shader_prog cube_face_sprog;
	shader_prog light_cube_sprog;

	GLuint loc_cube_face_pos;
	GLuint loc_cube_face_col;
	GLuint loc_cube_face_st;
	GLuint loc_cube_face_normal;

	GLuint loc_light_cube_pos;

	vertices cube_face;
	vertices light_cube;

	Matrix4f cube_model;
	Matrix4f view;
	Matrix4f proj;
	Vector3f light_pos;
	Vector3f light_col;
	Vector3f amb_light_pwr;
	Vector3f spec_light_col;
	float light_pwr;

	void draw_cube_face();
	void destroy_cube_face();

public:
	uos();
	~uos();

	bool init();
	bool init_cube_face();
	bool init_light_cube();

	void destroy();

	//bool prepare_shader_prog();
	void draw();


	void set_proj(const Matrix4f &proj);
	void set_view(const Matrix4f &view);
	void set_cube_model(const Matrix4f &model);
	void set_light_pos(const Vector3f &light_pos);
	void set_light_col(const Vector3f &light_col);
	void set_light_pwr(const float pwr);
	void set_amb_light_pwr(const Vector3f &light_pwr);
	void set_spec_light_col(const Vector3f &spec_col);
};