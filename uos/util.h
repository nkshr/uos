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
bool check_gl(const char *place);
void set_cube_vertices(const float length, float *buf);

void set_vec3(const float v0, const float v1, const float v2, float * _buf);
void set_vec4(const float v0, const float v1, const float v2, const float v3, float * _buf);

struct observer {
	Vector3f center;
	Vector3f up;
	Vector3f eye;

	Vector3f f, r, u;

	Matrix4f proj;
	Matrix4f view;

	observer() {

	}

	void update() {
		f = (center - eye).normalized();
		r = f.cross(up).normalized();
		u = r.cross(f).normalized();

		view << r(0), r(1), r(2), -r.dot(eye),
			u(0), u(1), u(2), -u.dot(eye),
			-f(0), -f(1), -f(2), f.dot(eye),
			0.f, 0.f, 0.f, 1.f;
	}

	void go_forward(const float step) {
		eye += step * f;
		center += step * f;
		update();
	}

	void go_backword(const float step) {
		eye -= step * f;
		center -= step * f;
		update();
	}

	void go_right(const float step) {
		eye += step * r;
		center += step * r;
		update();
	}

	void go_left(const float step) {
		eye -= step * r;
		center -= step * r;
		update();
	}

	void go_up(const float step) {
		eye += step * u;
		center += step * u;
		update();
	}

	void go_down(const float step) {
		eye -= step * u;
		center -= step * u;
		update();
	}

	void turn_left(const float step) {
		Matrix3f R = AngleAxisf(step, u).toRotationMatrix();
		Vector3f _f = center - eye;
		_f = R * _f;
		center = _f + eye;
		update();
	}

	void turn_right(const float step) {
		Matrix3f R = AngleAxisf(-step, u).toRotationMatrix();
		Vector3f _f = center - eye;
		_f = R * _f;
		center = _f + eye;
		update();
	}

	void turn_up(const float step) {
		Matrix3f R = AngleAxisf(step, r).toRotationMatrix();
		Vector3f _f = center - eye;
		_f = R * _f;
		center = _f + eye;
		up = R * up;
		update();
	}

	void turn_down(const float step) {
		Matrix3f R = AngleAxisf(-step, r).toRotationMatrix();
		Vector3f _f = center - eye;
		_f = R * _f;
		center = _f + eye;
		up = R * up;
		update();
	}
};

struct Phong_light {
	float pwr;
	Vector3f pos;
	Vector3f col;
	Vector3f amb_light_pwr;
	Vector3f spec_light_col;

	Phong_light(): pwr(1000), col(1.f, 1.f, 1.f),
		amb_light_pwr(0.3f, 0.3f, 0.3f), spec_light_col(1.f, 1.f, 1.f) {
	};
};