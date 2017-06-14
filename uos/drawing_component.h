#pragma once
class c_draw_comp {
protected:
	char *vsname;
	char *tcsname;
	char *tesname;
	char *gsname;
	char *fsname;
	char *csname;
	
	Matrix4f model;
	static Matrix4f view;
	static Matrix4f proj;
	static Matrix4f vp;
	Matrix4f mvp;

	static s_Phong_light light;
	GLint loc_pos;
	GLint loc_col;
	GLint loc_st;
	GLint loc_normal;

	GLuint vbuf_id;
	GLuint cbuf_id;
	GLuint ibuf_id;
	GLuint nbuf_id;

	c_shader_prog sprog;

	int num_vertices;
	int num_prim_vertices;
	int num_prims;
	float *poss;
	float *cols;
	float *normals;
	float *prim_normals;
	int *indices;

	int prim_type;

	void calc_prim_normals();
	virtual bool set_buffers() = 0;
	virtual bool set_uniforms() = 0;
	virtual bool get_attrib_locs() = 0;
	virtual void gen_buffers() = 0;

public:
	bool init();
	virtual void draw() = 0;

	static void set_view(const Matrix4f &view);
	static void set_proj(const Matrix4f &proj);
	static void set_light(const s_Phong_light &light);

	void set_model(const Matrix4f &model);
	virtual ~c_draw_comp();
};

class c_underwater_comp : public c_draw_comp {
protected:
	GLint loc_atten_coef;

	GLuint abuf_id;

	float *atten_coefs;
	float *absorp_coefs;

	virtual bool set_uniforms();
	virtual bool get_attrib_locs();
	
public:
	c_underwater_comp();
	virtual ~c_underwater_comp();
};

class c_underwater_cube :public c_underwater_comp{
protected:
	virtual bool set_buffers();
	virtual void gen_buffers();

public:
	c_underwater_cube();
	virtual void draw();
};

class c_light_cube :public c_draw_comp {
protected:
	virtual bool set_uniforms();
	virtual bool get_attrib_locs();
	virtual void gen_buffers();
	virtual bool set_buffers();

public:
	c_light_cube();
	virtual void draw();
	virtual ~c_light_cube();
};


class c_wire_plane: public c_draw_comp {
protected:
	virtual bool set_uniforms();
	virtual bool get_attrib_locs();
	virtual void gen_buffers();
	virtual bool set_buffers();

public:
	c_wire_plane();
	virtual void draw();
	virtual ~c_wire_plane();
};

class c_underwater_grids : public c_underwater_comp{
protected:
	int num_grids_along_x;
	int num_grids_along_y;

	virtual void gen_buffers();
	virtual bool set_buffers();

public:
	c_underwater_grids();
	virtual void draw();
	~c_underwater_grids();
};