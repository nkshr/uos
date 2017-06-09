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

#define NUM_CUBE_VERTICES 36
#define CUBE_LEN 1500.f
#define LIGHT_CUBE_LEN 200.f
#define PLANE_LEN 10000.f
#define PLANE_CELL_LEN 100.f

#include "util.h"
#include "math.h"
#include "drawing_component.h"

s_Phong_light c_draw_comp::light;
Matrix4f c_draw_comp::view;
Matrix4f c_draw_comp::proj;
Matrix4f c_draw_comp::vp;

void c_draw_comp::set_view(const Matrix4f &_view) {
	view = _view;
}

void c_draw_comp::set_proj(const Matrix4f &_proj) {
	proj = _proj;
}

void c_draw_comp::set_light(const s_Phong_light &_light) {
	light = _light;
}

void c_draw_comp::set_model(const Matrix4f &model) {
	this->model = model;
}

c_cube::c_cube() {
	strcpy(vsname, "cube.vs");
	strcpy(fsname, "cube.fs");
}

bool c_cube::init() {
	if (!sprog.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if (!sprog.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sprog.create_prog())
		return false;

	loc_pos = sprog.get_attrib_loc("pos_model");
	loc_col = sprog.get_attrib_loc("col");
	loc_normal = sprog.get_attrib_loc("normal_model");

	sprog.use();
	comp.num_vertices = NUM_CUBE_VERTICES;
	comp.num_prims = NUM_CUBE_VERTICES / 3;
	comp.poss = new  float[comp.num_vertices * 3];
	set_cube_vertices(CUBE_LEN, comp.poss);

	glGenBuffers(1, &comp.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.poss, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	comp.indices = new uint[comp.num_vertices];
	for (int i = 0; i < comp.num_vertices; ++i) {
		comp.indices[i] = i;
	}

	comp.cols = new float[comp.num_vertices * 3];
	set_vec3(1.f, 0.f, 0.f, comp.cols);
	for (int i = 1; i < 6; ++i) {
		set_vec3(1.f, 0.f, 0.f, NULL);
	}

	for (int i = 6; i < 12; ++i) {
		set_vec3(0.f, 1.f, 0.f, NULL);
	}

	for (int i = 12; i < 18; ++i) {
		set_vec3(0.f, 0.f, 1.f, NULL);
	}

	for (int i = 18; i < 24; ++i) {
		set_vec3(1.f, 1.f, 0.f, NULL);
	}

	for (int i = 24; i < 30; ++i) {
		set_vec3(1.f, 0.f, 1.f, NULL);
	}

	for (int i = 30; i < 36; ++i) {
		set_vec3(0.f, 1.f, 1.f, NULL);
	}

	glGenBuffers(1, &comp.cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.cols, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);

	comp.prim_normals = new float[comp.num_prims * 3];
	calc_prim_normals(comp);

	comp.normals = new float[comp.num_vertices * 3];
	for (int i = 0; i < comp.num_prims; ++i) {
		int j = i * 3;
		int k = i * 9;

		for (int l = 0; l < 3; ++l) {
			comp.normals[k++] = comp.prim_normals[j];
			comp.normals[k++] = comp.prim_normals[j + 1];
			comp.normals[k++] = comp.prim_normals[j + 2];
		}
	}

	glGenBuffers(1, &comp.nbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.nbuf_id);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_normal);
	glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	return check_gl("c_cube::init");
}

void c_cube::draw() {
	sprog.use();
	sprog.set_mat4("model", model.data());
	sprog.set_mat4("view", view.data());
	sprog.set_mat4("proj", proj.data());
	sprog.set_vec3("light_pos_world", light.pos(0), light.pos(1), light.pos(2));
	sprog.set_vec3("light_col", light.col(0), light.col(1), light.col(2));
	sprog.set_vec3("amb_light_pwr", light.amb_light_pwr(0), light.amb_light_pwr(1), light.amb_light_pwr(2));
	sprog.set_vec3("spec_light_col", light.spec_light_col(0), light.spec_light_col(1), light.spec_light_col(2));
	sprog.set_val("light_pwr", light.pwr);

	glDrawArrays(GL_TRIANGLES, 0, comp.num_vertices);
}

void c_cube::destroy() {
	glDeleteBuffers(1, &comp.vbuf_id);
	glDeleteBuffers(1, &comp.cbuf_id);
	glDeleteBuffers(1, &comp.ibuf_id);
	delete comp.poss;
	delete comp.cols;
	delete comp.normals;
	delete comp.prim_normals;
}

c_cube::~c_cube() {
	destroy();
}

c_light_cube::c_light_cube() {
	strcpy(vsname, "light_cube.vs");
	strcpy(fsname, "light_cube.fs");
}

bool c_light_cube::init() {
	if (!sprog.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if (!sprog.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sprog.create_prog())
		return false;

	loc_pos = sprog.get_attrib_loc("pos_model");
	sprog.use();

	comp.num_vertices = NUM_CUBE_VERTICES;
	comp.num_prims = NUM_CUBE_VERTICES / 3;
	comp.poss = new float[comp.num_vertices * 3];
	set_cube_vertices(LIGHT_CUBE_LEN, comp.poss);

	glGenBuffers(1, &comp.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	comp.indices = new uint[comp.num_vertices];
	return check_gl("c_light_cube::init");
}

void c_light_cube::draw() {
	sprog.use();
	sprog.set_vec3("light_col", light.col(0), light.col(1), light.col(2));
	sprog.set_val("light_pwr", light.pwr);

	Matrix4f light_model;
	light_model << 1.f, 0.f, 0.f, light.pos(0),
		0.f, 1.f, 0.f, light.pos(1),
		0.f, 0.f, 1.f, light.pos(2),
		0.f, 0.f, 0.f, 1.f;

	Matrix4f mvp = proj * view * light_model;
	sprog.set_mat4("mvp", mvp.data());
	glDrawArrays(GL_LINE_STRIP, 0, comp.num_vertices);
}

void c_light_cube::destroy() {
	glDeleteBuffers(1, &comp.vbuf_id);
	glDeleteBuffers(1, &comp.cbuf_id);
	glDeleteBuffers(1, &comp.ibuf_id);
	delete comp.poss;
}

c_light_cube::~c_light_cube() {
	destroy();
}

c_wire_cube::c_wire_cube() {
	strcpy(vsname, "line.vs");
	strcpy(gsname, "line.gs");
	strcpy(fsname, "line.fs");
}

bool c_wire_cube::init() {
	if (!sprog.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if (!sprog.create_shader(gsname, GL_GEOMETRY_SHADER))
		return false;
	if (!sprog.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sprog.create_prog())
		return false;

	sprog.use();
	loc_pos = sprog.get_attrib_loc("pos_model");
	loc_col = sprog.get_attrib_loc("col");

	comp.num_vertices = NUM_CUBE_VERTICES;
	comp.num_prims = NUM_CUBE_VERTICES / 3;
	comp.poss = new  float[comp.num_vertices * 3];
	set_cube_vertices(CUBE_LEN, comp.poss);

	glGenBuffers(1, &comp.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	comp.cols = new float[comp.num_vertices * 3];
	for (int i = 0; i < comp.num_vertices * 3; ++i)
		comp.cols[i] = 1.f;
	glGenBuffers(1, &comp.cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.cols, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);
	return check_gl("c_wire_cube::init");
}

void c_wire_cube::draw() {
	sprog.use();
	Matrix4f mvp = proj * view * model;
	sprog.set_mat4("mvp", mvp.data());
	//glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINE_STRIP, 0, comp.num_vertices);
}

void c_wire_cube::destroy() {
	glDeleteBuffers(1, &comp.vbuf_id);
	glDeleteBuffers(1, &comp.cbuf_id);
	glDeleteBuffers(1, &comp.ibuf_id);
	delete comp.poss;
	delete comp.cols;
}

c_wire_cube::~c_wire_cube() {
	destroy();
}

c_wire_plane::c_wire_plane() {
	strcpy(vsname, "line.vs");
	strcpy(gsname, "line.gs");
	strcpy(fsname, "line.fs");
}

bool c_wire_plane::init() {
	if (!sprog.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if (!sprog.create_shader(gsname, GL_GEOMETRY_SHADER))
		return false;
	if (!sprog.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sprog.create_prog())
		return false;

	loc_pos = sprog.get_attrib_loc("pos_model");
	loc_col = sprog.get_attrib_loc("col");
	sprog.use();
	check_gl("c_wire_plane::init0");

	const float half_len = PLANE_LEN * 0.5f;
	const float cell_len = PLANE_CELL_LEN;
	comp.num_vertices = 4 * static_cast<int>(PLANE_LEN / PLANE_CELL_LEN);
	comp.num_prims = comp.num_vertices / 2;
	comp.poss = new float[comp.num_vertices * 3];
	for (int i = 0; i < comp.num_vertices/2; ++i) {
		const int j = i * 3;
		const int k = i / 2;
		if (i % 2 == 0) {
			comp.poss[j] = half_len;
			comp.poss[j + 1] = 0.f;
			comp.poss[j + 2] = k * cell_len - half_len;
		}
		else {
			comp.poss[j] = -half_len;
			comp.poss[j + 1] = 0.f;
			comp.poss[j + 2] = k * cell_len - half_len;
		}
	}

	for (int i = comp.num_vertices / 2; i < comp.num_vertices; ++i) {
		const int j = i * 3;
		const int k = i / 2 - comp.num_vertices / 4;
		if (i % 2 == 0) {
			comp.poss[j] = k * cell_len - half_len;
			comp.poss[j + 1] = 0.f;
			comp.poss[j + 2] = half_len;
		}
		else {
			comp.poss[j] = k * cell_len - half_len;
			comp.poss[j + 1] = 0.f;
			comp.poss[j + 2] = -half_len;
		}
	}

	glGenBuffers(1, &comp.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	comp.cols = new float[comp.num_vertices * 3];
	for (int i = 0; i < comp.num_vertices * 3; ++i) {
		comp.cols[i] = 1.f;
	}

	glGenBuffers(1, &comp.cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, comp.cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * comp.num_vertices,
		comp.cols, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_col);

	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);
	return check_gl("c_wire_plane::init");
}

void c_wire_plane::draw() {
	sprog.use();
	Matrix4f mvp = proj * view;
	sprog.set_mat4("mvp", mvp.data());
	glDrawArrays(GL_LINES, 0, comp.num_vertices);
}

void c_wire_plane::destroy() {
	glDeleteBuffers(1, &comp.vbuf_id);
	glDeleteBuffers(1, &comp.cbuf_id);
	delete comp.poss;
	delete comp.cols;
};

c_wire_plane::~c_wire_plane() {
	destroy();
}

