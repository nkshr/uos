#pragma once
class c_shader_prog {
public:
	c_shader_prog();
	~c_shader_prog();

	bool create_shader(const char *fname, GLenum stype);
	bool create_prog();
	void destroy();
	void use();

	int get_attrib_loc(const char *param);

	void set_vec3(const char *param, const float a, const float b, const float c);
	void set_val(const char *param, const float val);
	void set_mat4(const char *param, const float *data);

	virtual bool init() { return true; }

protected:
	GLuint vs;
	GLuint gs;
	GLuint fs;
	GLuint program;
	GLuint vao;

	GLint loc_pos;
	GLint loc_col;
	GLint loc_st;
	GLint loc_normal;
	GLint loc_atten_coef;
	char *vsname;
	char *gsname;
	char *fsname;
};

class c_underwater_sprog : public c_shader_prog {
public:
	c_underwater_sprog();
	~c_underwater_sprog();

	virtual bool init();
};
bool check_gl(const char *place);
void set_cube_vertices(const float length, float *buf);

void set_vec3(const float v0, const float v1, const float v2, float * _buf);
void set_vec4(const float v0, const float v1, const float v2, const float v3, float * _buf);

struct s_observer {
	Vector3f center;
	Vector3f up;
	Vector3f eye;

	Vector3f f, r, u;

	Matrix4f proj;
	Matrix4f view;

	s_observer() {

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

struct s_Phong_light {
	bool bamb;
	bool bdiffuse;
	bool bspec;
	bool batten;

	float pwr;
	Vector3f pos;
	Vector3f col;
	Vector3f amb_light_pwr;
	Vector3f spec_light_col;

	s_Phong_light(): bamb(true), bdiffuse(true), bspec(true), batten(true), pwr(1000), col(1.f, 1.f, 1.f),
		amb_light_pwr(0.3f, 0.3f, 0.3f), spec_light_col(1.f, 1.f, 1.f) {
	};
};

class c_dmsg {
public:
	char name[1024];
	c_dmsg(const char *name) {
		strcpy(this->name, name);
		cerr << "Entering " << name << endl;
	}

	~c_dmsg() {
		cerr << "Exitinig " << name << endl;
	}
};

inline bool is_range(const float &min, const float &max, const float &val) {
	if (min <= val && val < max) {
		return true;
	}
	return false;
}
