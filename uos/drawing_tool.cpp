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
#include "drawing_tool.h"
//#include "shader.h"

GLuint vertex_array_id;
GLint loc_mvp;
GLint loc_pos;
GLint loc_col;
GLint loc_tex;
GLint loc_st;
GLint loc_btex;

shader_prog sp;

const char *vsname = "uos.vs";
const char *fsname = "uos.fs";
const char *gsname = "uos.gs";
bool init_drawing_tool() {
	glGenVertexArrays(1, &vertex_array_id);

	if (!sp.create_shader(vsname, GL_VERTEX_SHADER))
		return false;
	if(!sp.create_shader(gsname, GL_GEOMETRY_SHADER))
		return false;
	if (!sp.create_shader(fsname, GL_FRAGMENT_SHADER))
		return false;

	if (!sp.create_prog())
		return false;

	loc_mvp = sp.get_uniform_loc("mvp");
	loc_pos = sp.get_attrib_loc("position");
	loc_col = sp.get_attrib_loc("color");
	loc_st = sp.get_attrib_loc("st");

	loc_tex = sp.get_uniform_loc("tsampler");
	loc_btex = sp.get_uniform_loc("btexture");
	sp.use();

	return true;
}

GLuint register_vertexs(const int num_vertexs, const vertex * vertexs) {
	GLuint vbuf;
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * num_vertexs, vertexs, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)0);

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE,
		sizeof(vertex), (void*)(sizeof(float) * 3));

	return vbuf;

}

GLuint register_tvertexs(const int num_vertexs, const tvertex *vertexs) {
	GLuint vbuf;
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tvertex) * num_vertexs, vertexs, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE,
		sizeof(tvertex), (void*)0);

	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE,
		sizeof(tvertex), (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(loc_st);
	glVertexAttribPointer(loc_st, 2, GL_FLOAT, GL_FALSE,
		sizeof(tvertex), (void*)(sizeof(float) * 7));

	return vbuf;
}

void draw_coord(const Matrix4f &mvp, const float a) {
	const vertex coord[6] = {
		{ 1.f, 0.f, 0.f,   a, 0.f, 0.f, a },
		{ 0.f, 0.f, 0.f,   a, 0.f, 0.f, a },
		{ 0.f, 1.f, 0.f, 0.f,   a, 0.f, a },
		{ 0.f, 0.f, 0.f, 0.f,   a, 0.f, a },
		{ 0.f, 0.f, 1.f, 0.f, 0.f,   a, a },
		{ 0.f, 0.f, 0.f, 0.f, 0.f,   a, a },
	};

	GLuint vertex_buf;
	glGenBuffers(1, &vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 6, coord, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)0);
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)(sizeof(float) * 3));

	//glUseProgram(program);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());
	glDrawArrays(GL_LINES, 0, 6);
}

void draw_line(const vertex &sv, const vertex &ev, const Matrix4f &mvp) {
	const vertex line[2] = {
		{ sv.x, sv.y, sv.z, sv.r, sv.g, sv.b, sv.alpha },
		{ ev.x, ev.y, ev.z, ev.r, ev.g, ev.b, ev.alpha }
	};

	GLuint vertex_buf;
	glGenBuffers(1, &vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)0);
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)(sizeof(float) * 3));

	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());
	glDrawArrays(GL_LINES, 0, 2);
}


void draw_triangle(const vertex * vertexs, const int num_triangles, const Matrix4f &mvp) {
	GLuint vertex_buf;
	glGenBuffers(1, &vertex_buf);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 3* num_triangles, vertexs, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc_pos);
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)0);
	glEnableVertexAttribArray(loc_col);
	glVertexAttribPointer(loc_col, 4, GL_FLOAT, GL_FALSE,
		sizeof(float) * 7, (void*)(sizeof(float) * 3));

	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());
	glDrawArrays(GL_TRIANGLES, 0, 3 * num_triangles);
}

void draw_cube(const float r, const float g, const float b, const Matrix4f &mvp) {
	glUniform1i(loc_btex, false);
	const static vertex cube[] = {
		{1.f, 1.f, 1.f, r, g, b, 1.f },
		{1.f, 1.f, -1.f, r, g, b, 1.f},
		{-1.f, 1.f, -1.f, r, g, b, 1.f},
		{-1.f, 1.f, 1.f, r, g, b, 1.f},
		{1.f, -1.f, 1.f, r, g, b, 1.f},
		{ 1.f, -1.f, -1.f, r, g, b, 1.f },
		{-1.f, -1.f, -1.f, r, g, b, 1.f},
		{ -1.f, -1.f, 1.f, r, g, b, 1.f },
	};

	GLuint vtx_buf = register_vertexs(8, cube);

	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());

	const static GLuint indexes[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		0, 1, 5,
		5, 4, 0,

		1, 2, 6,
		6, 5, 1,

		2, 3, 7,
		7, 6, 2,

		3, 0, 4,
		4, 7, 3,
	};

	GLuint ibuf;
	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, indexes, GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void draw_triangles(const GLsizei num_elements, const vertex * vertexs, const Matrix4f &mvp) {
	glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);
}


void draw_texture(const int num_vertexs, const tvertex *vertexs,
	Mat &texture, const Matrix4f &mvp) {

	glUniform1i(loc_btex, true);
	glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, (const GLfloat*)mvp.data());

	GLuint vbuf = register_tvertexs(num_vertexs, vertexs);

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);

	check_gl_error("drawing_too.cpp", 230);

	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.cols,
		texture.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, (void*)texture.ptr<uchar>(0));
	
	check_gl_error("drawing_too.cpp", 236);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform1i(loc_tex, 0);


	glBindVertexArray(vertex_array_id);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_TRIANGLES, 0, num_vertexs);
	//glBindVertexArray(0);

}

