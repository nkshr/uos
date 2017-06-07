#pragma once

class c_uos {
private:
	c_cube cube;
	c_wire_cube wire_cube;
	c_light_cube light_cube;
	c_wire_plane wire_plane;

	s_Phong_light light;

	float light_pwr;

public:
	c_uos();
	~c_uos();

	bool init();

	void destroy();

	void draw();


	void set_proj(const Matrix4f &proj);
	void set_view(const Matrix4f &view);
	void set_cube_model(const Matrix4f &model);
	void set_light(const s_Phong_light &light);
};