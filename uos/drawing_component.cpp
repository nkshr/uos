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
#define CUBE_LEN 1.0f
#define LIGHT_CUBE_LEN 0.1f
#define PLANE_LEN 100.f
#define PLANE_CELL_LEN 0.1f
#define NUM_TRI_VERTICES 3
#define NUM_GRIDS_ALONG_X 10
#define NUM_GRIDS_ALONG_Y 10
#define GRID_LEN 0.1
#define NUM_QUAD_VERTICES 4

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

void c_draw_comp::calc_prim_normals() {
	for (int i = 0; i < num_prims; ++i) {
		int j = i * 3;
		int *_indices = &indices[j];
		const int ipos0 = _indices[0] * 3;
		float *pos0 = &poss[ipos0];
		const int ipos1 = _indices[1] * 3;
		float *pos1 = &poss[ipos1];
		const int ipos2 = _indices[2] * 3;
		float *pos2 = &poss[ipos2];

		cross(pos2[0] - pos0[0], pos2[1] - pos0[1], pos2[2] - pos0[2],
			pos1[0] - pos0[0], pos1[1] - pos0[1], pos1[2] - pos0[2],
			prim_normals[j], prim_normals[j + 1], prim_normals[j + 2]);
		normalize(prim_normals[j], prim_normals[j + 1], prim_normals[j + 2]);
	}
}

bool c_underwater_comp::get_attrib_locs() {
	loc_pos = sprog.get_attrib_loc("pos_model");
	loc_col = sprog.get_attrib_loc("col");
	loc_normal = sprog.get_attrib_loc("normal_model");
	loc_atten_coef = sprog.get_attrib_loc("atten_coef");
	return check_gl("c_underwater_comp::get_attrib_locs");
}

bool c_underwater_comp::set_uniforms() {
	sprog.set_mat4("model", model.data());
	sprog.set_mat4("view", view.data());
	sprog.set_mat4("proj", proj.data());
	sprog.set_vec3("light_pos_world", light.pos(0), light.pos(1), light.pos(2));
	sprog.set_vec3("light_col", light.col(0), light.col(1), light.col(2));
	sprog.set_vec3("amb_light_pwr", light.amb_light_pwr(0), light.amb_light_pwr(1), light.amb_light_pwr(2));
	sprog.set_vec3("spec_light_col", light.spec_light_col(0), light.spec_light_col(1), light.spec_light_col(2));
	sprog.set_val("light_pwr", light.pwr);
	sprog.set_val("bamb", light.bamb);
	sprog.set_val("bdiffuse", light.bdiffuse);
	sprog.set_val("bspec", light.bspec);
	sprog.set_val("batten", light.batten);
	return check_gl("c_underwater_comp::set_uniforms");
}

bool c_draw_comp::init() {
	if (!sprog.init(vsname, tcsname, tesname, gsname, fsname, csname))
		return false;
	if (!get_attrib_locs())
		return false;
	gen_buffers();
	if (!set_buffers())
		return false;
	return true;
}

c_draw_comp::~c_draw_comp() {
	delete vsname;
	delete tcsname;
	delete tesname;
	delete gsname;
	delete fsname;
	delete csname;

	glDeleteBuffers(1, &vbuf_id);
	glDeleteBuffers(1, &cbuf_id);
	glDeleteBuffers(1, &ibuf_id);

	delete poss;
	delete cols;
	delete normals;
	delete prim_normals;
}

c_underwater_comp::c_underwater_comp(){
	vsname = new char[1024];
	fsname = new char[1024];

	strcpy(vsname, "underwater.vs");
	strcpy(fsname, "underwater.fs");

	tcsname = NULL;
	tesname = NULL;
	gsname = NULL;
	csname = NULL;
}


c_underwater_comp::~c_underwater_comp() {
	delete absorp_coefs;
}

c_underwater_cube::c_underwater_cube():c_underwater_comp()
{	
	num_vertices = NUM_CUBE_VERTICES;
	num_prim_vertices = NUM_TRI_VERTICES;
	num_prims = num_vertices / num_prim_vertices;
}

void c_underwater_cube::gen_buffers() {
	poss = new float[num_vertices * 3];
	set_cube_vertices(CUBE_LEN, poss);

	cols = new float[num_vertices * 3];
	set_vec3(1.f, 0.f, 0.f, cols);
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

	indices = new int[num_vertices];
	for (int i = 0; i < num_vertices; ++i) {
		indices[i] = i;
	}

	prim_normals = new float[num_prims * 3];
	calc_prim_normals();

	normals = new float[num_vertices * 3];
	for (int i = 0; i < num_prims; ++i) {
		int j = i * 3;
		int k = i * 9;

		for (int l = 0; l < 3; ++l) {
			normals[k++] = prim_normals[j];
			normals[k++] = prim_normals[j + 1];
			normals[k++] = prim_normals[j + 2];
		}
	}

	atten_coefs = new float[num_vertices];
	absorp_coefs = new float[num_vertices];
	for (int i = 0; i < num_vertices; ++i) {
		float h, s, v;
		int j = i * 3;
		convert_rgb_to_hsv(cols[j], cols[j + 1], cols[j + 2], h, s, v);
		float wl = 1180.f - map_val(360.f, 0.f, 780.f, 400.f, h);
		absorp_coefs[i] = map_val(780.f, 400.f, 1.f, 0.01f, wl);
		//cout << h << ", " << absorp_coefs[i] << endl;
		//absorp_coefs[i] = 1.f;
	}
}

bool c_underwater_cube::set_buffers() {
	
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		cols, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &nbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, nbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_normal);
	glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &abuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, abuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices,
		absorp_coefs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_atten_coef);
	glVertexAttribPointer(loc_atten_coef, 1, GL_FLOAT, GL_FALSE, 0, 0);
	return check_gl("c_underwater_cube::init_buffers");
}

void c_underwater_cube::draw() {
	sprog.use();
	set_uniforms();
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

c_light_cube::c_light_cube() {
	prim_type = GL_TRIANGLES;
	vsname = new char[1024];
	fsname = new char[1024];

	strcpy(vsname, "light_cube.vs");
	strcpy(fsname, "light_cube.fs");

	tcsname = NULL;
	tesname = NULL;
	gsname = NULL;
	csname = NULL;

	num_vertices = NUM_CUBE_VERTICES;
	num_prims = NUM_CUBE_VERTICES / 3;
}

bool c_light_cube::set_uniforms() {
	sprog.set_vec3("light_col", light.col(0), light.col(1), light.col(2));
	sprog.set_val("light_pwr", light.pwr);

	Matrix4f light_model;
	light_model << 1.f, 0.f, 0.f, light.pos(0),
		0.f, 1.f, 0.f, light.pos(1),
		0.f, 0.f, 1.f, light.pos(2),
		0.f, 0.f, 0.f, 1.f;

	Matrix4f mvp = proj * view * light_model;
	sprog.set_mat4("mvp", mvp.data());
	return check_gl("c_light_cube::set_uniforms");
}

bool c_light_cube::get_attrib_locs() {
	loc_pos = sprog.get_attrib_loc("pos_model");
	return true;
}

void c_light_cube::gen_buffers() {
	poss = new float[num_vertices * 3];
	set_cube_vertices(LIGHT_CUBE_LEN, poss);
}

bool c_light_cube::set_buffers() {

	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	return check_gl("c_light_cube::init");
}

void c_light_cube::draw() {
	sprog.use();
	set_uniforms();
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}

c_light_cube::~c_light_cube() {
}



c_wire_plane::c_wire_plane() {
	vsname = new char[1024];
	gsname = new char[1024];
	fsname = new char[1024];

	strcpy(vsname, "line.vs");
	strcpy(gsname, "line.gs");
	strcpy(fsname, "line.fs");

	tcsname = NULL;
	tesname = NULL;
	csname = NULL;
}

bool c_wire_plane::set_uniforms() {
	Matrix4f mvp = proj * view;
	sprog.set_mat4("mvp", mvp.data());
	return check_gl("c_wire_plane::set_uniforms");
}

bool c_wire_plane::get_attrib_locs() {
	loc_pos = sprog.get_attrib_loc("pos_model");
	loc_col = sprog.get_attrib_loc("col");
	return true;
}

void c_wire_plane::gen_buffers() {
	const float half_len = PLANE_LEN * 0.5f;
	const float cell_len = PLANE_CELL_LEN;
	num_vertices = 4 * static_cast<int>(PLANE_LEN / PLANE_CELL_LEN);
	num_prims = num_vertices / 2;
	poss = new float[num_vertices * 3];
	for (int i = 0; i < num_vertices / 2; ++i) {
		const int j = i * 3;
		const int k = i / 2;
		if (i % 2 == 0) {
			poss[j] = half_len;
			poss[j + 1] = 0.f;
			poss[j + 2] = k * cell_len - half_len;
		}
		else {
			poss[j] = -half_len;
			poss[j + 1] = 0.f;
			poss[j + 2] = k * cell_len - half_len;
		}
	}

	for (int i = num_vertices / 2; i < num_vertices; ++i) {
		const int j = i * 3;
		const int k = i / 2 - num_vertices / 4;
		if (i % 2 == 0) {
			poss[j] = k * cell_len - half_len;
			poss[j + 1] = 0.f;
			poss[j + 2] = half_len;
		}
		else {
			poss[j] = k * cell_len - half_len;
			poss[j + 1] = 0.f;
			poss[j + 2] = -half_len;
		}
	}

	cols = new float[num_vertices * 3];
	for (int i = 0; i < num_vertices * 3; ++i) {
		cols[i] = 1.f;
	}
}

bool c_wire_plane::set_buffers() {
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		cols, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_col);

	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);
	return check_gl("c_wire_plane::init");
}

void c_wire_plane::draw() {
	sprog.use();
	set_uniforms();
	glDrawArrays(GL_LINES, 0, num_vertices);
}

c_wire_plane::~c_wire_plane() {
}

c_underwater_grids::c_underwater_grids():c_underwater_comp() {
	num_grids_along_x = NUM_GRIDS_ALONG_X;
	num_grids_along_y = NUM_GRIDS_ALONG_Y;
	num_prims = num_grids_along_x * num_grids_along_y;
	num_vertices = NUM_QUAD_VERTICES * num_prims;
}

void c_underwater_grids::gen_buffers() {
	const int offsetx = num_grids_along_x >> 1;
	const int offsety = num_grids_along_y >> 1;
	const float hstep = 360.f / num_prims;
	poss = new float[num_vertices * 3];
	normals = new float[num_vertices * 3];
	cols = new float[num_vertices * 3];
	for (int x = 0; x < num_grids_along_x; ++x) {
		for (int y = 0; y < num_grids_along_y; ++y) {
			const int iposs = (x + y * num_grids_along_x) * 12;
			poss[iposs] = static_cast<float>(x - offsetx);
			poss[iposs + 1] = static_cast<float>(y - offsety);
			poss[iposs + 2] = 0.f;

			poss[iposs + 3] = static_cast<float>(x - offsetx);
			poss[iposs + 4] = static_cast<float>(y + 1 - offsety);
			poss[iposs + 5] = 0.f;

			poss[iposs + 6] = static_cast<float>(x + 1 - offsetx);
			poss[iposs + 7] = static_cast<float>(y + 1 - offsety);
			poss[iposs + 8] = 0.f;

			poss[iposs + 9] = static_cast<float>(x + 1 - offsetx);
			poss[iposs + 10] = static_cast<float>(y - offsety);
			poss[iposs + 11] = 0.f;

			const float h = (x + y * num_grids_along_x) * hstep;
			const float s = 1.f;//get_rand(1.f, 0.f);
			const float v = 1.f;//get_rand(1.f, 0.f);
			float *pcols = &cols[iposs];
			float *pnormals = &normals[iposs];
			for (int i = 0; i < NUM_QUAD_VERTICES; ++i) {
				*pcols = h;
				++pcols;
				*pcols = s;
				++pcols;
				*pcols = v;
				++pcols;

				*pnormals = 0.f;
				++pnormals;
				*pnormals = 0.f;
				++pnormals;
				*pnormals = 1.f;
				++pnormals;
			}

		}
	}

	atten_coefs = new float[num_vertices];
	absorp_coefs = new float[num_vertices];
	for (int i = 0; i < num_vertices; ++i) {
		int j = i * 3;
		absorp_coefs[i] = calc_absorp_coef(cols[j]);
		const float h = cols[j];
		const float s = cols[j + 1];
		const float v = cols[j + 2];

		convert_hsv_to_rgb(h, s, v, cols[j], cols[j + 1], cols[j + 2]);
		atten_coefs[i] = absorp_coefs[i];
		//cout << atten_coefs[i] << " : "  << cols[j] 
		//	<< ", " << cols[j + 1] << ", " << cols[j + 2] << endl;
	}
}

bool c_underwater_grids::set_buffers() {	
	glGenBuffers(1, &vbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		poss, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &nbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, nbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_normal);
	glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
	glGenBuffers(1, &cbuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices,
		cols, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glGenBuffers(1, &abuf_id);
	glBindBuffer(GL_ARRAY_BUFFER, abuf_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices,
		atten_coefs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(loc_atten_coef);
	glVertexAttribPointer(loc_atten_coef, 1, GL_FLOAT, GL_FALSE, 0, 0);
	return check_gl("c_underwater_grids::init_buffers");
}

void c_underwater_grids::draw() {
	sprog.use();
	set_uniforms();
	glDrawArrays(GL_QUADS, 0, num_vertices);
}

c_underwater_grids::~c_underwater_grids() {
	
}
