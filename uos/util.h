#pragma once
//struct vertex {
//	float x, y, z;
//	float r, g, b, alpha;
//
//	vertex(const float x, const float y, const float z,
//		const float r, const float g, const float b, const float alpha) :x(x), y(y),
//		z(z), r(r), g(g), b(b), alpha(alpha) {
//	}
//};
//
//struct tvertex {
//	float x, y, z;
//	float r, g, b, alpha;
//	float s, t;
//};

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