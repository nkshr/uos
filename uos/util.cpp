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
//#include "drawing_tool.h"
//#include "shader.h"

char * load_text(const char *fname) {
	FILE * pf = fopen(fname, "rb");
	if (!pf) {
		return NULL;
	}

	fseek(pf, 0, SEEK_END);
	int size = ftell(pf);
	rewind(pf);

	char *txt = (char*)malloc(size + 1);
	fread(txt, 1, size, pf);
	txt[size] = 0;
	fclose(pf);

	return txt;
}

shader_prog::shader_prog(){
}

bool shader_prog::create_prog() {
	program = glCreateProgram();

	if(vs)
		glAttachShader(program, vs);
	if(gs)
		glAttachShader(program, gs);
	if(fs)
		glAttachShader(program, fs);
	
	glLinkProgram(program);

	int link_stat;
	glGetProgramiv(program, GL_LINK_STATUS, &link_stat);

	if (link_stat == GL_FALSE) {
		cerr << "Error : Couldn't create shader program. " << endl;
		int info_log_length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length > 0) {
			char * info_log = new char[info_log_length];
			int chars_written = 0;
			glGetProgramInfoLog(program, info_log_length, &chars_written, info_log);
			cerr << info_log << endl;
			delete info_log;
		}

		return false;
	}
	return true;
}


void shader_prog::destroy() {
	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDetachShader(program, gs);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
	glDeleteShader(program);
}

shader_prog::~shader_prog() {
	destroy();
}

int shader_prog::get_attrib_loc(const char *param) {
	return glGetAttribLocation(program, param);
}

void shader_prog::set_vec3(const char *param, const float v0, const float v1, const float v2) {
	GLuint loc =  glGetUniformLocation(program, param);
	glUniform3f(loc, v0, v1, v2);
}

void shader_prog::set_val(const char *param, const float val) {
	GLuint loc = glGetUniformLocation(program, param);
	glUniform1f(loc, val);
}

void shader_prog::set_mat4(const char *param, const float *data) {
	GLuint loc = glGetUniformLocation(program, param);
	glUniformMatrix4fv(loc, 1, GL_FALSE, data);
}

void shader_prog::use() {
	glUseProgram(program);
}

bool shader_prog::create_shader(const char * fname, GLenum stype) {
	char *ssrc = load_text(fname);
	if (!ssrc) {
		cerr << "Error : Couldn't open " << fname << "." << endl;
		return NULL;
	}

	int shader = glCreateShader(stype);
	glShaderSource(shader, 1, &ssrc, NULL);
	glCompileShader(shader);
	delete ssrc;

	GLint cmp_stat;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &cmp_stat);
	if (cmp_stat == GL_FALSE) {
		cerr << "Error : Couldn't compile " << fname << "." << endl;
		int info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		if (info_log_length > 0) {
			char * info_log = new char[info_log_length];
			int chars_written = 0;
			glGetShaderInfoLog(shader, info_log_length, &chars_written, info_log);
			cout << info_log << endl;
			delete info_log;
		}

		glDeleteShader(shader);
		shader = 0;
	}

	switch (stype) {
	case GL_VERTEX_SHADER:
		vs = shader;
		break;
	case GL_GEOMETRY_SHADER:
		gs = shader;
		break;
	case GL_FRAGMENT_SHADER:
		fs = shader;
		break;
	}
	return static_cast<bool>(shader);
}

void check_gl_error(const char *file, int line) {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
		err = glGetError();
	}
}


void set_vec3(const float v0, const float v1, const float v2, float * _buf) {
	static float *buf;
	if (_buf != NULL)
		buf = _buf;

	*buf = v0;
	buf++;
	*buf = v1;
	buf++;
	*buf = v2;
	buf++;
}

void set_vec4(const float v0, const float v1, const float v2, const float v3, float * _buf) {
	static float *buf;
	if (_buf != NULL)
		buf = _buf;

	*buf = v0;
	buf++;
	*buf = v1;
	buf++;
	*buf = v2;
	buf++;
	*buf = v3;
	buf++;
}



void set_cube_vertices(const float length, float *buf) {
	const float half_len = length * 0.5f;
	//face0
	set_vec3(half_len, half_len, half_len, buf);//0
	set_vec3(-half_len, half_len, half_len, NULL);//3
	set_vec3(-half_len, half_len, -half_len, NULL);//2

	set_vec3(-half_len, half_len, -half_len, NULL);//2
	set_vec3(half_len, half_len, -half_len, NULL);//1
	set_vec3(half_len, half_len, half_len, NULL);//0

	//face1
	set_vec3(half_len, -half_len, half_len, NULL);//4
	set_vec3(half_len, -half_len, -half_len, NULL);//5
	set_vec3(-half_len, -half_len, -half_len, NULL);//6

	set_vec3(-half_len, -half_len, -half_len, NULL);//6
	set_vec3(-half_len, -half_len, half_len, NULL);//7
	set_vec3(half_len, -half_len, half_len, NULL);//4

	//face2
	set_vec3(half_len, half_len, half_len, NULL);//0
	set_vec3(half_len, half_len, -half_len, NULL);//1
	set_vec3(half_len, -half_len, -half_len, NULL);//5

	set_vec3(half_len, -half_len, -half_len, NULL);//5
	set_vec3(half_len, -half_len, half_len, NULL);//4
	set_vec3(half_len, half_len, half_len, NULL);//0

	//face3
	set_vec3(half_len, half_len, -half_len, NULL);//1
	set_vec3(-half_len, half_len, -half_len, NULL);//2
	set_vec3(-half_len, -half_len, -half_len, NULL);//6

	set_vec3(-half_len, -half_len, -half_len, NULL);//6
	set_vec3(half_len, -half_len, -half_len, NULL);//5
	set_vec3(half_len, half_len, -half_len, NULL);//1

	//face4
	set_vec3(-half_len, half_len, -half_len, NULL);//2
	set_vec3(-half_len, half_len, half_len, NULL);//3
	set_vec3(-half_len, -half_len, half_len, NULL);//7

	set_vec3(-half_len, -half_len, half_len, NULL);//7
	set_vec3(-half_len, -half_len, -half_len, NULL);//6
	set_vec3(-half_len, half_len, -half_len, NULL);//2

	//face5
	set_vec3(-half_len, half_len, half_len, NULL);//3
	set_vec3(half_len, half_len, half_len, NULL);//0
	set_vec3(half_len, -half_len, half_len, NULL);//4

	set_vec3(half_len, -half_len, half_len, NULL);//4
	set_vec3(-half_len, -half_len, half_len, NULL);//7
	set_vec3(-half_len, half_len, half_len, NULL);//3
}