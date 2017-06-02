#pragma once
class shader_prog {
public:
	shader_prog();
	~shader_prog();

	bool create_shader(const char *fname, GLenum stype);
	bool create_prog();
	void destroy();
	void bind();
	void use();

	int get_attrib_loc(const char *param);

	void set_vec3(const char *param, const float a, const float b, const float c);
	void set_val(const char *param, const float val);
	void set_mat4(const char *param, const float *data);

private:
	GLuint vs;
	GLuint gs;
	GLuint fs;
	GLuint program;
	GLuint vao;
};
void check_gl_error(const char *file, int line);
void set_cube_vertices(const float length, float *buf);

void set_vec3(const float v0, const float v1, const float v2, float * _buf);
void set_vec4(const float v0, const float v1, const float v2, const float v3, float * _buf);