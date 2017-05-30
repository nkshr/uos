#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace std;

#include "shader.h"

char * load_text(const char *fname) {
	FILE * pf = fopen(fname, "rb");
	if (!pf) {
		return NULL;
	}

	fseek(pf, 0, SEEK_END);
	int size = ftell(pf);
	rewind(pf);

	char *txt = (char*)malloc(size+1);
	fread(txt, 1, size, pf);
	txt[size] = 0;
	fclose(pf);

	return txt;
}

shader_prog::shader_prog() {
}

bool shader_prog::create_prog() {
	program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glAttachShader(program, gs);

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

	int cmp_stat;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &cmp_stat);
	if (cmp_stat == GL_FALSE) {
		cerr << "Error : Couldn't compile " << fname << "." << endl;
		int info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		if(info_log_length > 0){
			char * info_log = new char[info_log_length];
			int chars_written = 0;
			glGetShaderInfoLog(shader, info_log_length, &chars_written, info_log);
			cout << info_log << endl;
			delete info_log;
		}

		glDeleteShader(shader);
		return false;
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

	delete ssrc;

	return true;
}