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


struct vector2f {
	float data[2];
};

struct vector3f {
	float data[3];
};

struct vector4f {
	float  data[4];
};

struct vector2ui {
	uint data[2];
};

struct vector3ui {
	uint data[3];
};

class vertexes{
protected:
	vector2f *tposs;
	vector3f *poss;
	vector3f *normals;
	vector4f *cols;

	int num_vertexes;
	int num_prims;

public:
	GLuint vbuf;
	GLuint cbuf;
	GLuint ibuf;
	GLuint nbuf;

	vertexes() {

	}

	virtual ~vertexes() {

	}

	void malloc(const int num_vertexes) {
		this->num_vertexes = num_vertexes;
		this->num_prims = num_vertexes;
		poss = new vector3f[num_vertexes];
		normals = new vector3f[num_vertexes];
		cols = new  vector4f[num_vertexes];
		tposs = new vector2f[num_vertexes];
	}

	void set_pos(const int ipos, const float x, const float y, const float z) {
		poss[ipos].data[0] = x;
		poss[ipos].data[1] = y;
		poss[ipos].data[2] = z;
	}

	void set_normal(const int inormal, const float x, const float y, const float z) {
		normals[inormal].data[0] = x;
		normals[inormal].data[1] = y;
		normals[inormal].data[2] = z;
	}

	void set_color(const int icol,
		const float r, const float g, const float b, const float a) {
		cols[icol].data[0] = r;
		cols[icol].data[1] = g;
		cols[icol].data[2] = b;
		cols[icol].data[3] = a;
	}

	int get_num_vertexes() {
		return num_vertexes;
	}

	int get_num_prims() {
		return num_prims;
	}

	vector3f* get_positions() {
		return poss;
	}

	vector3f* get_normals() {
		return normals;
	}

	vector4f* get_colors() {
		return cols;
	}



	void destroy() {
		delete poss;
		delete normals;
		delete cols;
		delete tposs;
	}

	void calc_normals_origin();
};

class tri_vertexes : public vertexes{
private:
	vector3f *tri_normals;
	vector3ui *indexes;
	
public:

	void set_triangle(const int itri, const int i0, const int i1, const int i2) {
		indexes[itri].data[0] = i0;
		indexes[itri].data[1] = i1;
		indexes[itri].data[2] = i2;
	}

	vector3f * get_tri_normals() {
		return tri_normals;
	}

	vector3ui* get_indexes() {
		return indexes;
	}


	void malloc(const int num_vertexes, const int num_tris) {
		vertexes::malloc(num_vertexes);
		this->num_prims = num_tris;
		tri_normals = new vector3f[num_tris];
		indexes = new vector3ui[num_tris];
	}

	void destroy() {
		vertexes::destroy();
		delete tri_normals;
		delete indexes;
	}

	tri_vertexes() {
	}

	~tri_vertexes() {
		destroy();
	}

	void calc_tri_normals();
	void calc_normals();
};

class line_vertexes:public vertexes{
private:
	vector2ui *indexes;
	
public:
	void malloc(const int num_vertexes, const int num_prims){
		vertexes::malloc(num_vertexes);
		this->num_prims = num_prims;

		indexes = new vector2ui[num_prims];
	}

	vector2ui* get_indexes() {
		return indexes;
	}

	void set_line(const int iline, const int i0, const int i1) {
		indexes[iline].data[0] = i0;
		indexes[iline].data[1] = i1;
	}

	void destroy() {
		vertexes::destroy();
		delete indexes;
	}

	line_vertexes() {

	}

	~line_vertexes() {

	}
};

inline void cross(const float a0, const float a1, const float a2,
	const float b0, const float b1, const float b2,
	float &c0, float &c1, float &c2) {
	c0 = a1 * b2 - a2 * b1;
	c1 = a2 * b0 - a0 * b2;
	c2 = a0 * b1 - a1 * b0;
}
inline float deg_to_rad(const float deg) {
	return static_cast<float>(EIGEN_PI) * deg / 180.f;
}