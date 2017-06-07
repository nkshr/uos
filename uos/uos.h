#pragma once

class uos {
private:
	c_cube cube;
	c_wire_cube wire_cube;
	c_light_cube light_cube;
	c_wire_plane wire_plane;

	Phong_light light;

	float light_pwr;

public:
	uos();
	~uos();

	bool init();

	void destroy();

	void draw();


	void set_proj(const Matrix4f &proj);
	void set_view(const Matrix4f &view);
	void set_cube_model(const Matrix4f &model);
	void set_light_pos(const Vector3f &light_pos);
	void set_light_col(const Vector3f &light_col);
	void set_light_pwr(const float pwr);
	void set_amb_light_pwr(const Vector3f &light_pwr);
	void set_spec_light_col(const Vector3f &spec_col);
};