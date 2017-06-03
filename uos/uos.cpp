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
#define CUBE_LEN 200

uos::uos() {
	cube_vsname = "cube.vs";
	cube_fsname = "cube.fs";
}

uos::~uos() {

}

bool uos::prepare_shader_prog() {
	if (!cube_sprog.create_shader(cube_vsname, GL_VERTEX_SHADER))
		return false;
	if (!cube_sprog.create_shader(cube_fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!cube_sprog.create_prog())
		return false;

	loc_pos = cube_sprog.get_attrib_loc("pos_model");
	loc_col = cube_sprog.get_attrib_loc("col");
	loc_st = cube_sprog.get_attrib_loc("st");
	loc_nml = cube_sprog.get_attrib_loc("normal_model");
	cube_sprog.bind();
	cube_sprog.use();
	return true;
}

void uos::draw() {
	draw_cube_face();
}


void uos::draw_cube_face() {
	//cube_sprog.bind();
	//cube_sprog.use();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void uos::destroy_cube_face() {
	glDeleteBuffers(1, &cube_face.vbuf_id);
	glDeleteBuffers(1, &cube_face.cbuf_id);
	glDeleteBuffers(1, &cube_face.ibuf_id);
}

bool uos::init() {
	if (!prepare_shader_prog())
		return false;

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

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE, 0, 0);

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

	glEnableVertexAttribArray(loc_nml);
	glVertexAttribPointer(loc_nml, 3, GL_FLOAT, GL_FALSE, 0, 0);

	return true;
}

void uos::destroy() {
	cube_sprog.destroy();
}

void uos::set_proj(const float  *proj) {
	cube_sprog.set_mat4("proj", proj);
	//light_sprog.set_mat4("proj", proj);
}

void uos::set_view(const float *light) {
	cube_sprog.set_mat4("view", light);
	//light_sprog.set_mat4("view", light);
}

void uos::set_cube_model(const  float *model) {
	cube_sprog.set_mat4("model", model);
}

void uos::set_light_pos(const float x, const float y, const float z) {
	cube_sprog.set_vec3("light_pos_world", x, y, z);
	//light_sprog.set_vec3("light_pos_world", x, y, z);
}

void uos::set_light_col(const float r, const float g, const float b) {
	cube_sprog.set_vec3("light_col", r, g, b);
}

void uos::set_amb_light_pwr(const float r, const float g, const float b) {
	cube_sprog.set_vec3("amb_light_pwr", r, g, b);
}

void uos::set_light_pwr(const float pwr) {
	cube_sprog.set_val("light_pwr", pwr);
}

void uos::set_spec_col(const float r, const float g, const float b) {
	cube_sprog.set_vec3("spec_light_col", r, g, b);
}