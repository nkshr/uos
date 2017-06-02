#pragma once
void get_proj(const float fx, const float fy,
	const float cx, const float cy, Matrix4f &P);
void get_frustum(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);
void get_ortho(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P);
void get_Rx(const float ang, Matrix4f &R);
void get_Ry(const float ang, Matrix4f &R);
void get_Rz(const float ang, Matrix4f &R);
void get_Sim3(const float s, const float xang, const float yang, const float zang,
	const float x, const float y, const float z, Matrix4f &Sim3);
void get_SE3_inv(const float xang, const float yang, const float zang, const float x, const float y, const float z, Matrix4f &SE3);

struct vertices{
	float *tposs;
	float *poss;
	float *normals;
	float *prim_normals;
	float *cols;
	uint *indices;
	int num_vertices;
	int num_prims;

	GLuint vbuf_id;
	GLuint cbuf_id;
	GLuint ibuf_id;
	GLuint nbuf_id;
};

void calc_prim_normals(vertices);

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