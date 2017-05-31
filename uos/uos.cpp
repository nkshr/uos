#include <iostream>
#include <string>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace Eigen;
using namespace cv;

#include "util.h"
#include "math.h"
#include "uos.h"

uos::uos() {
	vsname = "uos.vs";
	gsname = "uos.gs";
	fsname = "uos.fs";

	cube_model = Matrix4f::Identity();
	view = Matrix4f::Identity();
	proj = Matrix4f::Identity();
}

uos::~uos() {

}

bool uos::prepare_shader_prog() {
	if (!sp.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if (!sp.create_shader(gsname, GL_GEOMETRY_SHADER))
		return false;
	if (!sp.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sp.create_prog())
		return false;

	loc_pos = sp.get_attrib_loc("position_model");
	loc_col = sp.get_attrib_loc("color");
	loc_st = sp.get_attrib_loc("st");
	loc_nml = sp.get_attrib_loc("normal_model");

	loc_model = sp.get_uniform_loc("model");
	loc_view = sp.get_uniform_loc("view");
	loc_proj = sp.get_uniform_loc("proj");
	loc_mvp = sp.get_uniform_loc("mvp");
	loc_mv = sp.get_uniform_loc("mv");

	loc_light_pos = sp.get_uniform_loc("light_position_world");
	loc_light_col = sp.get_uniform_loc("light_color");
	loc_light_power = sp.get_uniform_loc("light_power");
	loc_ambient_light_power = sp.get_uniform_loc("ambient_light_power");
	loc_specular_color = sp.get_uniform_loc("specular_color");

	loc_tex = sp.get_uniform_loc("tsampler");
	loc_btex = sp.get_uniform_loc("btexture");
	sp.use();
	return true;
}

void uos::draw() {
	draw_cube_face();
}

void uos::init_light() {
	glUniform3f(loc_light_pos, light_pos[0], light_pos[1], light_pos[2]);
	glUniform3f(loc_light_col, light_col[0], light_col[1], light_col[2]);
	glUniform1f(loc_light_power, light_power);
	glUniform3f(loc_ambient_light_power, ambient_light_power_r, ambient_light_power_g, ambient_light_power_b);
	glUniform3f(loc_specular_color, specular_r, specular_g, specular_b);
}

void uos::init_cube_face() {
	float half_len = 300;
	cube_face.malloc(36, 12);
	//face0
	cube_face.set_pos(0, half_len, half_len, half_len);//0
	cube_face.set_pos(1, -half_len, half_len, half_len);//3
	cube_face.set_pos(2, -half_len, half_len, -half_len);//2

	cube_face.set_pos(3, -half_len, half_len, -half_len);//2
	cube_face.set_pos(4, half_len, half_len, -half_len);//1
	cube_face.set_pos(5, half_len, half_len, half_len);//0

	//face1
	cube_face.set_pos(6, half_len, -half_len, half_len);//4
	cube_face.set_pos(7, half_len, -half_len, -half_len);//5
	cube_face.set_pos(8, -half_len, -half_len, -half_len);//6

	cube_face.set_pos(9, -half_len, -half_len, -half_len);//6
	cube_face.set_pos(10, -half_len, -half_len, half_len);//7
	cube_face.set_pos(11, half_len, -half_len, half_len);//4

	//face2
	cube_face.set_pos(12, half_len, half_len, half_len);//0
	cube_face.set_pos(13, half_len, half_len, -half_len);//1
	cube_face.set_pos(14, half_len, -half_len, -half_len);//5

	cube_face.set_pos(15, half_len, -half_len, -half_len);//5
	cube_face.set_pos(16, half_len, -half_len, half_len);//4
	cube_face.set_pos(17, half_len, half_len, half_len);//0

	//face3
	cube_face.set_pos(18, half_len, half_len, -half_len);//1
	cube_face.set_pos(19, -half_len, half_len, -half_len);//2
	cube_face.set_pos(20, -half_len, -half_len, -half_len);//6

	cube_face.set_pos(21, -half_len, -half_len, -half_len);//6
	cube_face.set_pos(22, half_len, -half_len, -half_len);//5
	cube_face.set_pos(23, half_len, half_len, -half_len);//1

	//face4
	cube_face.set_pos(24, -half_len, half_len, -half_len);//2
	cube_face.set_pos(25, -half_len, half_len, half_len);//3
	cube_face.set_pos(26, -half_len, -half_len, half_len);//7

	cube_face.set_pos(27, -half_len, -half_len, half_len);//7
	cube_face.set_pos(28, -half_len, -half_len, -half_len);//6
	cube_face.set_pos(29, -half_len, half_len, -half_len);//2

	 //face5

	cube_face.set_pos(30, -half_len, half_len, half_len);//3
	cube_face.set_pos(31, half_len, half_len, half_len);//0
	cube_face.set_pos(32, half_len, -half_len, half_len);//4

	cube_face.set_pos(33, half_len, -half_len, half_len);//4
	cube_face.set_pos(34, -half_len, -half_len, half_len);//7
	cube_face.set_pos(35, -half_len, half_len, half_len);//3

	glGenBuffers(1, &cube_face.vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector3f) * cube_face.get_num_vertexes(),
		cube_face.get_positions(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vector3f), 0);

	for (int i = 0; i < 6; ++i) {
		cube_face.set_color(i, 1.f, 0.f, 0.f, 1.f);
	}

	for (int i = 6; i < 12; ++i) {
		cube_face.set_color(i, 0.f, 1.f, 0.f, 1.f);
	}

	for (int i = 12; i < 18; ++i) {
		cube_face.set_color(i, 0.f, 0.f, 1.f, 1.f);
	}

	for (int i = 18; i < 24; ++i) {
		cube_face.set_color(i, 1.f, 1.f, 0.f, 1.f);
	}

	for (int i = 24; i < 30; ++i) {
		cube_face.set_color(i, 1.f, 0.f, 1.f, 1.f);
	}

	for (int i = 30; i < 36; ++i) {
		cube_face.set_color(i, 0.f, 1.f, 1.f, 1.f);
	}

	glGenBuffers(1, &cube_face.cbuf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.cbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector4f) * cube_face.get_num_vertexes(),
		cube_face.get_colors(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE, sizeof(vector4f), 0);

	for (int i = 0; i < cube_face.get_num_prims(); ++i) {
		int j = 3 * i;
		cube_face.set_triangle(i, j, j + 1, j + 2);
		//cube_face.set_triangle(i, j++, j++, j++);
	}

	glGenBuffers(1, &cube_face.ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_face.ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vector3ui) * cube_face.get_num_prims(),
		cube_face.get_indexes(), GL_STATIC_DRAW);

	cube_face.calc_tri_normals();

	/*for (int i = 0; i < cube_face.get_num_prims(); ++i) {
	cout << cube_face.get_tri_normals()[i].data[0] << ", " << cube_face.get_tri_normals()[i].data[1]
	<< ", " << cube_face.get_tri_normals()[i].data[2] << endl;
	}*/

	//cube_face.calc_normals_origin();
	cube_face.calc_normals();
	for (int i = 0; i < cube_face.get_num_vertexes(); ++i) {
		cout << cube_face.get_normals()[i].data[0] << ", " << cube_face.get_normals()[i].data[1]
			<< ", " << cube_face.get_normals()[i].data[2] << endl;
	}

	glGenBuffers(1, &cube_face.nbuf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.nbuf);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vector3f) * cube_face.get_num_vertexes(),
		cube_face.get_normals(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_nml);
	glVertexAttribPointer(loc_nml, 3, GL_FLOAT, GL_FALSE, sizeof(vector3f), 0);
	cube_face.destroy();
}

void uos::init_cube_wire() {
	cube_wire.malloc(8, 12);
	cube_wire.set_pos(0, 1.f, 1.f, 1.f);
	cube_wire.set_pos(1, 1.f, 1.f, -1.f);
	cube_wire.set_pos(2, -1.f, 1.f, -1.f);
	cube_wire.set_pos(3, -1.f, 1.f, 1.f);
	cube_wire.set_pos(4, 1.f, -1.f, 1.f);
	cube_wire.set_pos(5, 1.f, -1.f, -1.f);
	cube_wire.set_pos(6, -1.f, -1.f, -1.f);
	cube_wire.set_pos(7, -1.f, -1.f, 1.f);

	glGenBuffers(1, &cube_wire.vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_wire.vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector3f) * cube_wire.get_num_vertexes(),
		cube_wire.get_positions(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vector3f), 0);

	for (int i = 0; i < cube_wire.get_num_vertexes(); ++i) {
		cube_wire.set_color(i, 0.f, 1.f, 0.f, 1.f);
	}

	glGenBuffers(1, &cube_wire.cbuf);
	glBindBuffer(GL_ARRAY_BUFFER, cube_wire.cbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vector4f) * cube_wire.get_num_vertexes(),
		cube_wire.get_colors(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE, sizeof(vector4f), 0);

	cube_wire.set_line(0, 0, 1);
	cube_wire.set_line(1, 1, 2);
	cube_wire.set_line(2, 2, 3);
	cube_wire.set_line(3, 3, 0);

	cube_wire.set_line(4, 4, 5);
	cube_wire.set_line(5, 5, 6);
	cube_wire.set_line(6, 6, 7);
	cube_wire.set_line(7, 7, 4);

	cube_wire.set_line(8, 0, 4);
	cube_wire.set_line(9, 1, 5);
	cube_wire.set_line(10, 2, 6);
	cube_wire.set_line(11, 3, 7);

	glGenBuffers(1, &cube_wire.ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_wire.ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vector3ui) * cube_wire.get_num_prims(),
		cube_wire.get_indexes(), GL_STATIC_DRAW);

}

void uos::draw_cube_face() {
	glUniform1i(loc_btex, false);
	init_light();
	glUniformMatrix4fv(loc_model, 1, GL_FALSE, cube_model.data());
	glUniformMatrix4fv(loc_view, 1, GL_FALSE, view.data());
	glUniformMatrix4fv(loc_proj, 1, GL_FALSE, proj.data());

	Matrix4f mv = view * cube_model;
	glUniformMatrix4fv(loc_mv, 1, GL_FALSE, mv.data());

	Matrix4f mvp = proj * mv;
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void uos::draw_cube_wire() {
	glUniform1i(loc_btex, false);

	glUniformMatrix4fv(loc_model, 1, GL_FALSE, cube_model.data());
	glUniformMatrix4fv(loc_view, 1, GL_FALSE, view.data());
	glUniformMatrix4fv(loc_proj, 1, GL_FALSE, proj.data());

	Matrix4f mv = view * cube_model;
	glUniformMatrix4fv(loc_mv, 1, GL_FALSE, mv.data());

	Matrix4f mvp = proj * mv;
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());

	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}

void uos::destroy_cube_face() {
	glDeleteBuffers(1, &cube_face.vbuf);
	glDeleteBuffers(1, &cube_face.cbuf);
	glDeleteBuffers(1, &cube_face.ibuf);
}

void uos::destroy_cube_wire() {
	glDeleteBuffers(1, &cube_wire.vbuf);
	glDeleteBuffers(1, &cube_wire.cbuf);
	glDeleteBuffers(1, &cube_wire.ibuf);
}

bool uos::init() {
	if (!prepare_shader_prog())
		return false;

	init_light();
	init_cube_face();
	//init_cube_wire();
	return true;
}

void uos::destroy() {
	sp.destroy();
}

void uos::set_proj(const Matrix4f &p) {
	proj = p;
}

void uos::set_view(const Matrix4f &v) {
	view = v;
}

void uos::set_cube_model(const  Matrix4f &m) {
	cube_model = m;
}

void uos::set_light_position(const Vector3f &light_pos) {
	this->light_pos = light_pos;
}

void uos::set_light_color(const Vector3f &light_col) {
	this->light_col = light_col;
}

void uos::set_ambient_light_power(const float r, const float g, const float b) {
	ambient_light_power_r = r;
	ambient_light_power_g = g;
	ambient_light_power_b = b;
}

void uos::set_light_power(const float light_power) {
	this->light_power = light_power;
}

void uos::set_specular_color(const float r, const float g, const float b) {
	specular_r = r;
	specular_g = g;
	specular_b = b;
}