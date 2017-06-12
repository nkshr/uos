#pragma once
class c_draw_comp {
protected:
	char vsname[1026];
	char gsname[1026];
	char fsname[1026];
	
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

	s_shader_prog sprog;
	s_vertices comp;

	int num_vertices;
	int num_prim_vertices;
	int num_prims;
	float *poss;
	float *cols;
	float *normals;
	float *prim_normals;

	int prim_type;

public:
	//c_draw_comp() {};
	virtual bool init() = 0;
	virtual void draw() = 0;
	virtual void destroy() = 0;
	//~c_draw_comp() {};

	static void set_view(const Matrix4f &view);
	static void set_proj(const Matrix4f &proj);
	static void set_light(const s_Phong_light &light);

	void set_model(const Matrix4f &model);
};

class c_underwater_comp : public c_draw_comp {
protected:
	GLint loc_atten_coef;

	GLuint abuf_id;

	float *atten_coefs;
	float *absorp_coefs;

public:
	c_underwater_comp();
	virtual bool init();
	virtual void draw();
	~c_underwater_comp();
};

class c_cube :public c_underwater_comp{
private:

public:
	c_cube();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_cube();

	//void enable_atten(const bool enbl);
	//void enable_amb_light(const bool enbl);
	//void enable_diffuse_light(const bool enbl);
	//void enable_spec_light(const bool enbl);


};

class c_light_cube :public c_draw_comp {
public:
	c_light_cube();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_light_cube();
};


class c_wire_cube : public c_draw_comp{
public:
	c_wire_cube();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_wire_cube();
};

class c_wire_plane: public c_draw_comp {
public:
	c_wire_plane();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_wire_plane();
};

class c_underwater_grids : public c_underwater_comp{
private:
	int num_grids_along_x;
	int num_grids_along_y;
	//float atten_coef;

public:
	c_underwater_grids();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_underwater_grids();
};