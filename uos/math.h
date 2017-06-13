#pragma once
void get_proj(const float fx, const float fy,
	const float cx, const float cy, Matrix4f &P);
void get_frustum(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);
void get_ortho(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);
void get_perspective(const float fov, const float aspect, const float near, const float far, Matrix4f &P);
void get_Rx(const float ang, Matrix4f &R);
void get_Ry(const float ang, Matrix4f &R);
void get_Rz(const float ang, Matrix4f &R);
void get_Sim3(const float s, const float xang, const float yang, const float zang,
	const float x, const float y, const float z, Matrix4f &Sim3);
void get_SE3_inv(const float xang, const float yang, const float zang, const float x, const float y, const float z, Matrix4f &SE3);

//struct s_vertices{
//	float *tposs;
//	float *poss;
//	float *normals;
//	float *prim_normals;
//	float *cols;
//	uint *indices;
//	int num_vertices;
//	int num_prims;
//
//	GLuint vbuf_id;
//	GLuint cbuf_id;
//	GLuint ibuf_id;
//	GLuint nbuf_id;
//};

//void calc_prim_normals(s_vertices vtxs);

inline void cross(const float a0, const float a1, const float a2,
	const float b0, const float b1, const float b2,
	float &c0, float &c1, float &c2) {
	c0 = a1 * b2 - a2 * b1;
	c1 = a2 * b0 - a0 * b2;
	c2 = a0 * b1 - a1 * b0;
}

inline void normalize(float &a, float &b, float &c) {
	float d = sqrtf(a * a + b * b + c * c);
	float id = 1.f / d;
	a *= id;
	b *= id;
	c *= id;
}

inline float deg_to_rad(const float deg) {
	return static_cast<float>(EIGEN_PI) * deg / 180.f;
}

void convert_rgb_to_hsv(const float r, const float g, const float b,
	float &h, float &s, float &v);
void convert_hsv_to_rgb(const float h, const float s, const float v,
	float &r, float &g, float &b);
float adjustment(const float color, const float  factor, const float gamma);
void convert_wavelength_to_rgb(const float gamma,
	const float wl, float &r, float &g, float &b);

inline float map_val(const float max_val, const float min_val,
	const float mmax_val, const float mmin_val, const float val) {
	return (val - min_val) * (mmax_val - mmin_val) / (max_val - min_val) + mmin_val;
}

float get_rand(const float max_val, const float min_val);

inline float calc_absorp_coef(const float r, const float g, const float b) {
	float h, s, v;
	convert_rgb_to_hsv(r, g, b, h, s, v);
	float wl = map_val(180.f, 0.f, 780.f, 400.f, abs(h - 180.f));
	const float absorp_coef = map_val(780.f, 400.f, 1.f, 0.01f, wl);
	cout << r << ", " << g << ", " << b << " :  " << h << " : " << absorp_coef << endl;
	return absorp_coef;
}

inline float calc_absorp_coef(const float h) {
	float wl = map_val(180.f, 0.f, 780.f, 400.f, abs(h - 180.f));
	const float absorp_coef = map_val(780.f, 400.f, 1.f, 0.01f, wl);
	//cout << h  << " : " << absorp_coef << endl;
	return absorp_coef;
}