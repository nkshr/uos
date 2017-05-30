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

	/*delete vssrc;
	delete fssrc;*/
}

shader_prog::~shader_prog() {
	destroy();
}

int shader_prog::get_attrib_loc(const char *param) {
	return glGetAttribLocation(program, param);
}

int shader_prog::get_uniform_loc(const char *param) {
	return glGetUniformLocation(program, param);
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