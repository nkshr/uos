#pragma once

class uos {
private:
	const char *vsname;
	const char *gsname;
	const char *fsname;

	shader_prog sp;

	GLuint loc_pos;
	GLuint loc_col;
	GLuint loc_st;
	GLuint loc_nml;

	GLuint loc_model;
	GLuint loc_view;
	GLuint loc_proj;
	GLuint loc_mv;
	GLuint loc_mvp;
	GLuint loc_light_pos;
	GLuint loc_light_col;
	GLuint loc_light_power;
	GLuint loc_tex;
	GLuint loc_btex;	

	Vector3f light_pos;
	Vector4f light_col;

	float light_power;

	tri_vertexes cube_face;
	line_vertexes cube_wire;

	Matrix4f cube_model;
	Matrix4f view;
	Matrix4f proj;

	//GLuint register_vertexs(const int num_vertexs, const uos::vertex *vertexs);
	//GLuint register_indexes(const int num_indexs, const GLuint *indexes);

	void init_light();
	void init_cube_face();
	void init_cube_wire();

	void draw_cube_face();
	void draw_cube_wire();

	void destroy_cube_face();
	void destroy_cube_wire();

public:
	uos();
	~uos();

	bool init();
	void destroy();

	bool prepare_shader_prog();
	void draw();


	void set_proj(const Matrix4f &p);
	void set_view(const Matrix4f &v);
	void set_cube_model(const Matrix4f &m);
	void set_light_position(const  Vector3f &light_pos);
	void set_light_color(const Vector4f &light_col);
	void set_light_power(const float light_power);
};