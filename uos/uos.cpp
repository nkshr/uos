#include <iostream>
#include <string>
#include <math.h>

#include <GL/glew.h>
#include <GL/glew.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace Eigen;
using namespace cv;

#include "util.h"
#include "math.h"
#include "uos.h"

#define NUM_CUBE_VERTICES 36
#define CUBE_LEN 1500
#define LIGHT_CUBE_LEN 200

uos::uos() {
	cube_face_vsname = "cube.vs";
	cube_face_fsname = "cube.fs";
	cube_wire_vsname = "line.vs";
	cube_wire_gsname = "line.gs";
	cube_wire_fsname = "line.fs";
	light_cube_vsname = "light_cube.vs";
	light_cube_fsname = "light_cube.fs";
}

uos::~uos() {

}

void uos::draw() {
	//draw_cube_face();
	draw_cube_wire();
	draw_light_cube();
}


void uos::draw_cube_face() {
	cube_face_sprog.use();
	cube_face_sprog.set_mat4("model", cube_model.data());
	cube_face_sprog.set_mat4("view", view.data());
	cube_face_sprog.set_mat4("proj", proj.data());
	cube_face_sprog.set_vec3("light_pos_world", light_pos(0), light_pos(1), light_pos(2));
	cube_face_sprog.set_vec3("light_col", light_col(0), light_col(1), light_col(2));
	cube_face_sprog.set_vec3("amb_light_pwr", amb_light_pwr(0), amb_light_pwr(1), amb_light_pwr(2));
	cube_face_sprog.set_vec3("spec_light_col", spec_light_col(0), spec_light_col(1), spec_light_col(2));
	cube_face_sprog.set_val("light_pwr", light_pwr);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void uos::draw_cube_wire() {
	cube_wire_sprog.use();
	Matrix4f mvp = proj * view * cube_model;
	cube_wire_sprog.set_mat4("mvp", mvp.data());
	glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, 0);
}

void uos::draw_light_cube() {
	light_cube_sprog.use();
	light_cube_sprog.set_vec3("light_col", light_col(0), light_col(1), light_col(2));
	light_cube_sprog.set_val("light_pwr", light_pwr);
	
	Matrix4f light_model;
	light_model << 1.f, 0.f, 0.f, light_pos(0),
		0.f, 1.f, 0.f, light_pos(1),
		0.f, 0.f, 1.f, light_pos(2),
		0.f, 0.f, 0.f, 1.f;

	Matrix4f mvp = proj * view * light_model;
	light_cube_sprog.set_mat4("mvp", mvp.data());
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void uos::destroy_cube_face() {
	glDeleteBuffers(1, &cube_face.vbuf_id);
	glDeleteBuffers(1, &cube_face.cbuf_id);
	glDeleteBuffers(1, &cube_face.ibuf_id);
	delete cube_face.poss;
	delete cube_face.indices;
	delete cube_face.cols;
	delete cube_face.normals;
	delete cube_face.prim_normals;
}

void uos::destroy_cube_wire() {
	glDeleteBuffers(1, &cube_wire.vbuf_id);
	glDeleteBuffers(1, &cube_wire.cbuf_id);
	glDeleteBuffers(1, &cube_wire.ibuf_id);
	delete cube_wire.poss;
	delete cube_wire.cols;
	delete cube_wire.indices;
}

void uos::destroy_light_cube() {
	glDeleteBuffers(1, &light_cube.vbuf_id);
	glDeleteBuffers(1, &light_cube.cbuf_id);
	glDeleteBuffers(1, &light_cube.ibuf_id);
	delete light_cube.poss;
	delete light_cube.indices;
}

bool uos::init() {
	if (!init_cube_face() || !init_light_cube() ||!init_cube_wire()) {
		return false;
	}
	return true;
}

bool uos::init_cube_face() {
	if (!cube_face_sprog.create_shader(cube_face_vsname, GL_VERTEX_SHADER))
		return false;
	if (!cube_face_sprog.create_shader(cube_face_fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!cube_face_sprog.create_prog())
		return false;

	loc_cube_face_pos = cube_face_sprog.get_attrib_loc("pos_model");
	loc_cube_face_col = cube_face_sprog.get_attrib_loc("col");
	loc_cube_face_st = cube_face_sprog.get_attrib_loc("st");
	loc_cube_face_normal = cube_face_sprog.get_attrib_loc("normal_model");
	cube_face_sprog.use();

	cube_face.num_vertices = NUM_CUBE_VERTICES;
	cube_face.num_prims = NUM_CUBE_VERTICES / 3;
	cube_face.poss = new float[cube_face.num_vertices * 3];
	set_cube_vertices(CUBE_LEN, cube_face.poss);

	//for (int i = 0; i < cube_face.num_vertices*3; i+=3) {
	//	cout << cube_face.poss[i] << ", " << cube_face.poss[i + 1] << ", " << cube_face.poss[i + 2] << endl;
	//}

	glGenBuffers(1, &cube_face.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cube_face.num_vertices,
		cube_face.poss, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_cube_face_pos);
	glVertexAttribPointer(loc_cube_face_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	cube_face.indices = new uint[cube_face.num_vertices];
	for (int i = 0; i < cube_face.num_vertices; ++i) {
		cube_face.indices[i] = i;
	}

	glGenBuffers(1, &cube_face.ibuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_face.ibuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * cube_face.num_vertices,
		cube_face.indices, GL_STATIC_DRAW);

	cube_face.cols = new float[cube_face.num_vertices * 4];
	set_vec4(1.f, 0.f, 0.f, 1.f, cube_face.cols);
	for (int i = 1; i < 6; ++i) {
		set_vec4(1.f, 0.f, 0.f, 1.f, NULL);
	}

	for (int i = 6; i < 12; ++i) {
		set_vec4(0.f, 1.f, 0.f, 1.f, NULL);
	}

	for (int i = 12; i < 18; ++i) {
		set_vec4(0.f, 0.f, 1.f, 1.f, NULL);
	}

	for (int i = 18; i < 24; ++i) {
		set_vec4(1.f, 1.f, 0.f, 1.f, NULL);
	}

	for (int i = 24; i < 30; ++i) {
		set_vec4(1.f, 0.f, 1.f, 1.f, NULL);
	}

	for (int i = 30; i < 36; ++i) {
		set_vec4(0.f, 1.f, 1.f, 1.f, NULL);
	}

	glGenBuffers(1, &cube_face.cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * cube_face.num_vertices,
		cube_face.cols, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_cube_face_col);
	glVertexAttribPointer(loc_cube_face_col, 4, GL_FLOAT, GL_FALSE, 0, 0);

	cube_face.prim_normals = new float[cube_face.num_prims * 3];
	calc_prim_normals(cube_face);

	cube_face.normals = new float[cube_face.num_vertices * 3];
	for (int i = 0; i < cube_face.num_prims; ++i) {
		int j = i * 3;
		int k = i * 9;

		for (int l = 0; l < 3; ++l) {
			cube_face.normals[k++] = cube_face.prim_normals[j];
			cube_face.normals[k++] = cube_face.prim_normals[j + 1];
			cube_face.normals[k++] = cube_face.prim_normals[j + 2];
		}
	}

	//for (int i = 0; i < cube_face.num_vertices; ++i) {
	//	int j = i * 3;
	//	cout << cube_face.normals[j] << ", " << cube_face.normals[j + 1] << ", " << cube_face.normals[j + 2] << endl;
	//}

	glGenBuffers(1, &cube_face.nbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_face.nbuf_id);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cube_face.num_vertices,
		cube_face.normals, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_cube_face_normal);
	glVertexAttribPointer(loc_cube_face_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	return true;
}

bool uos::init_light_cube() {
	if (!light_cube_sprog.create_shader(light_cube_vsname, GL_VERTEX_SHADER))
		return false;
	if (!light_cube_sprog.create_shader(light_cube_fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!light_cube_sprog.create_prog())
		return false;

	loc_light_cube_pos = light_cube_sprog.get_attrib_loc("pos_model");
	light_cube_sprog.use();

	light_cube.num_vertices = NUM_CUBE_VERTICES;
	light_cube.num_prims = NUM_CUBE_VERTICES / 3;
	light_cube.poss = new float[light_cube.num_vertices * 3];
	set_cube_vertices(LIGHT_CUBE_LEN, light_cube.poss);

	glGenBuffers(1, &light_cube.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, light_cube.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * light_cube.num_vertices,
		light_cube.poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_light_cube_pos);
	glVertexAttribPointer(loc_light_cube_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	light_cube.indices = new uint[light_cube.num_vertices];
	for (int i = 0; i < light_cube.num_vertices; ++i)
		light_cube.indices[i] = i;

	glGenBuffers(1, &light_cube.ibuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cube.ibuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * light_cube.num_vertices,
		light_cube.indices, GL_STATIC_DRAW);

	return true;
}

bool uos::init_cube_wire() {
	if (!cube_wire_sprog.create_shader(cube_wire_vsname, GL_VERTEX_SHADER))
		return false;
	if (!cube_wire_sprog.create_shader(cube_wire_gsname, GL_GEOMETRY_SHADER))
		return false;
	if (!cube_wire_sprog.create_shader(cube_wire_fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!cube_wire_sprog.create_prog())
		return false;

	loc_cube_wire_pos = cube_wire_sprog.get_attrib_loc("pos_model");
	loc_cube_wire_col = cube_wire_sprog.get_attrib_loc("col");
	cube_wire_sprog.use();

	cube_wire.num_vertices = NUM_CUBE_VERTICES;
	cube_wire.num_prims = NUM_CUBE_VERTICES / 3;
	cube_wire.poss = new  float[cube_wire.num_vertices * 3];
	set_cube_vertices(CUBE_LEN, cube_wire.poss);

	glGenBuffers(1, &cube_wire.vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_wire.vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cube_wire.num_vertices,
		cube_wire.poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_cube_wire_pos);
	glVertexAttribPointer(loc_cube_wire_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	cube_wire.indices = new uint[cube_wire.num_vertices];
	for (int i = 0; i < cube_wire.num_vertices; ++i)
		cube_wire.indices[i] = i;

	glGenBuffers(1, &cube_wire.ibuf_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_wire.ibuf_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * cube_wire.num_vertices,
		cube_wire.indices, GL_STATIC_DRAW);

	cube_wire.cols = new float[cube_wire.num_vertices * 3];
	for (int i = 0; i < cube_wire.num_vertices * 3; ++i)
		cube_wire.cols[i] = 1.f;
	glGenBuffers(1, &cube_wire.cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_wire.cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * cube_wire.num_vertices,
		cube_wire.cols, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_cube_wire_col);
	glVertexAttribPointer(loc_cube_wire_col, 3, GL_FLOAT, GL_FALSE, 0, 0);
	return true;
}

void uos::destroy() {
	cube_face_sprog.destroy();
}

void uos::set_proj(const Matrix4f &proj) {
	this->proj = proj;
}

void uos::set_view(const Matrix4f &view) {
	this->view = view;
}

void uos::set_cube_model(const  Matrix4f &model) {
	cube_model = model;
}

void uos::set_light_pos(const Vector3f &light_pos) {
	this->light_pos = light_pos;
}

void uos::set_light_col(const Vector3f &light_col) {
	this->light_col = light_col;
}

void uos::set_amb_light_pwr(const Vector3f &amb_light_pwr) {
	this->amb_light_pwr = amb_light_pwr;
}

void uos::set_spec_light_col(const Vector3f &spec_light_col) {
	this->spec_light_col = spec_light_col;
}

void uos::set_light_pwr(const float light_pwr) {
	this->light_pwr = light_pwr;
}

