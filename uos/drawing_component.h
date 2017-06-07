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

	static Phong_light light;
	GLuint loc_pos;
	GLuint loc_col;
	GLuint loc_st;
	GLuint loc_normal;

	shader_prog sprog;
	vertices comp;

public:
	c_draw_comp() {};
	virtual bool init() = 0;
	virtual void draw() = 0;
	virtual void destroy() = 0;
	~c_draw_comp() {};

	static void set_view(const Matrix4f &view);
	static void set_proj(const Matrix4f &proj);
	static void set_light(const Phong_light &light);

	void set_model(const Matrix4f &model);
};

class c_cube :public c_draw_comp{
public:
	c_cube();
	virtual bool init();
	virtual void draw();
	virtual void destroy();
	~c_cube();
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