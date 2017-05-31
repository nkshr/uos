#pragma once
class shader_prog {
public:
	shader_prog();
	~shader_prog();

	bool create_shader(const char *fname, GLenum stype);
	bool create_prog();
	void destroy();
	void use();

	int get_attrib_loc(const char *param);
	int get_uniform_loc(const char *param);

private:
	GLuint vs;
	GLuint gs;
	GLuint fs;
	GLuint program;
};
void check_gl_error(const char *file, int line);