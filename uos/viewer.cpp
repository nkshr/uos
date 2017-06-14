#include <Windows.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>


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
#include "drawing_component.h"


const int win_width = 1280;
const int win_height = 960;
const char * texture_name = "golddiag.jpg";
const float trans_step = 0.1f;
const float rot_step = 1.f;

bool bdraw_cube = true;
bool bdraw_light_cube = true;
bool bdraw_wire_plane = true;
bool bdraw_grids = true;

s_observer obsv;
s_Phong_light light;

mutex mtx;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_W:
			obsv.go_forward(trans_step);
			break;
		case GLFW_KEY_S:
			obsv.go_backword(trans_step);
			break;
		case GLFW_KEY_D:
			obsv.go_right(trans_step);
			break;
		case GLFW_KEY_A:
			obsv.go_left(trans_step);
			break;
		case GLFW_KEY_UP:
			if (GLFW_MOD_SHIFT == mods)
				obsv.go_up(trans_step);
			else
				obsv.turn_up(deg_to_rad(rot_step));
			break;
		case GLFW_KEY_DOWN:
			if (GLFW_MOD_SHIFT == mods)
				obsv.go_down(trans_step);
			else
				obsv.turn_down(deg_to_rad(rot_step));
			break;
		case GLFW_KEY_LEFT:
			obsv.turn_left(deg_to_rad(rot_step));
			break;
		case GLFW_KEY_RIGHT:
			obsv.turn_right(deg_to_rad(rot_step));
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		default:
			break;
		}
	}
}

void parse_args(char *args, vector<char*> &toks) {
	if (args) {
		toks.clear();
		toks.reserve(24);
	}

	char *tok = strtok(args, " ");
	if (!tok)
		return;

	char *_tok = new char[strlen(tok)+1];
	memcpy(static_cast<void*>(_tok), static_cast<void*>(tok), strlen(tok)+1);	
	toks.push_back(_tok);
	parse_args(NULL, toks);
}

void cmd_receiver(GLFWwindow *window) {
	while (true) {
		mtx.lock();
		if (glfwWindowShouldClose(window)) {
			mtx.unlock();
			break;
		}

		mtx.unlock();
		char buf[1024];
		cin.getline(buf, 1024);
		vector<char *> toks;
		parse_args(buf, toks);
		//for (int i = 0; i < toks.size(); ++i) {
		//	std::cout << toks[i] << endl;
		//}

		vector<char*>::iterator it_toks = toks.begin();
		mtx.lock();
		if (strcmp(*it_toks, "set") == 0) {
			while ((++it_toks) != toks.end()) {
				if (strcmp(*it_toks, "draw_cube") == 0) {
					if (++it_toks == toks.end())
						break;
					if (strcmp(*it_toks, "y") == 0) {
						bdraw_cube = true;
					}
					else if( strcmp(*it_toks, "n") == 0){
						bdraw_cube = false;
					}
				}
				else if (strcmp(*it_toks, "draw_light_cube") == 0) {
					if (++it_toks == toks.end())
						break;
					if (strcmp(*it_toks, "y") == 0) {
						bdraw_light_cube = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						bdraw_light_cube = false;
					}
				}
				else if (strcmp(*it_toks, "draw_grids") == 0) {
					if (++it_toks == toks.end())
						break;
					if (strcmp(*it_toks, "y") == 0) {
						bdraw_grids = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						bdraw_grids = false;
					}
				}
				else if (strcmp(*it_toks, "obsv_pos") == 0) {
					if ((++it_toks) == toks.end())
						break;
					obsv.eye(0) = static_cast<float>(atof(*it_toks));
					
					if ((++it_toks) == toks.end())
						break;
					obsv.eye(1) = static_cast<float>(atof(*it_toks));
					
					if ((++it_toks) == toks.end())
						break;
					obsv.eye(2) = static_cast<float>(atof(*it_toks));
					obsv.update();
				}
				else if (strcmp(*it_toks, "light_pwr") == 0) {
					++it_toks;
					light.pwr = static_cast<float>(atof(*it_toks));
				}
				else if (strcmp(*it_toks, "light_pos") == 0) {
					if ((++it_toks) == toks.end())
						break;
					light.pos(0) = static_cast<float>(atof(*it_toks));

					if ((++it_toks) == toks.end())
						break;
					light.pos(1) = static_cast<float>(atof(*it_toks));

					if ((++it_toks) == toks.end())
						break;
					light.pos(2) = static_cast<float>(atof(*it_toks));
				}
				else if (strcmp(*it_toks, "amb_light") == 0) {
					++it_toks;
					if (strcmp(*it_toks, "y") == 0) {
						light.bamb = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						light.bamb = false;
					}
				}
				else if (strcmp(*it_toks, "diffuse_light") == 0) {
					++it_toks;
					if (strcmp(*it_toks, "y") == 0) {
						light.bdiffuse = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						light.bdiffuse = false;
					}
				}
				else if (strcmp(*it_toks, "spec_light") == 0) {
					++it_toks;
					if (strcmp(*it_toks, "y") == 0) {
						light.bspec = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						light.bspec = false;
					}
				}
				else if (strcmp(*it_toks, "atten") == 0) {
					++it_toks;
					if (strcmp(*it_toks, "y") == 0) {
						light.batten = true;
					}
					else if (strcmp(*it_toks, "n") == 0) {
						light.batten = false;
					}
				}
				else if (strcmp(*it_toks, "original") == 0) {
					light.bamb = true;
					light.bdiffuse = false;
					light.bspec = false;
					light.batten = false;

					light.amb_light_pwr = Vector3f(1.f, 1.f, 1.f);
				}
				else if (strcmp(*it_toks, "amb_light_pwr") == 0) {
					if (++it_toks == toks.end())
						break;
					light.amb_light_pwr[0] = static_cast<float>(atof(*it_toks));

					if (++it_toks == toks.end())
						break;
					light.amb_light_pwr[1] = static_cast<float>(atof(*it_toks));

					if (++it_toks == toks.end())
						break;
					light.amb_light_pwr[2] = static_cast<float>(atof(*it_toks));
				}
				else {
					cerr << "WARNING : undefined command received. " 
						<< *it_toks << endl;
				}
			}
		}
		else if (strcmp(*it_toks, "get") == 0) {
			++it_toks;
			while (it_toks != toks.end()) {
				if (strcmp(*it_toks, "proj") == 0){
					cout << obsv.proj << endl;
					++it_toks;
				}

				if (strcmp(*it_toks, "view") == 0) {
					cout << obsv.view << endl;
					++it_toks;
				}

				if (strcmp(*it_toks, "light_pos") == 0) {
					cout << light.pos << endl;
					++it_toks;
				}

				if (strcmp(*it_toks, "light_pwr") == 0) {
					cout << light.pwr << endl;
					++it_toks;
				}

			}
		}
		else if (strcmp(*it_toks, "quit") == 0) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			++it_toks;
		}
		else {
			cerr << "WARNING : undefined command received. "
				<< *it_toks << endl;
		}


		mtx.unlock();
	}
}

int main(int argc, char ** argv) {
	GLFWwindow* window;

	//glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(win_width, win_height, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Error : " << glewGetErrorString(err);
		exit(EXIT_FAILURE);
	}

	cout << "Info : Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	
	glfwSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//initialization
	c_underwater_cube cube;
	if (!cube.init())
		return false;

	c_light_cube light_cube;
	if (!light_cube.init())
		return false;

	c_wire_plane wire_plane;
	if (!wire_plane.init())
		return false;

	c_underwater_grids grids;
	if (!grids.init())
		return false;


	light.pos = Vector3f(1.f, 1.f, 1.f);
	light.col = Vector3f(1.f, 1.f, 1.f);
	light.pwr = 2;
	light.amb_light_pwr = Vector3f(0.2f, 0.2f, 0.2f);
	light.spec_light_col = Vector3f(0.3f, 0.3f, 0.3f);

	Mat texture = imread(texture_name);
	if (texture.empty()) {
		cerr << "Error : Couldn't read " << texture_name << "." << endl;
		return false;
	}

	obsv.center = Vector3f(0.f, 0.f, 0.f);
	obsv.up = Vector3f(0.f, 1.f, 0.f);

	get_perspective(deg_to_rad(60.f), 640.f/ 480.f, 0.1f, 10, obsv.proj);
	c_draw_comp::set_proj(obsv.proj);

	obsv.eye = Vector3f(0.f, 0.f, 3.f);
	obsv.update();
	float ang = 0.f;

	thread t(cmd_receiver, window);

	while (true) {
		if (glfwWindowShouldClose(window)) {
			break;
		}
		int frame_width, frame_height;
		glfwGetFramebufferSize(window, &frame_width, &frame_height);
		glViewport(0, 0, frame_width, frame_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 0.f);

		mtx.lock();
		c_draw_comp::set_light(light);
		c_draw_comp::set_view(obsv.view);

		Matrix4f cube_model;
		get_Sim3(0.f, deg_to_rad(ang), deg_to_rad(0), 0.f, 0.f, 0.f, 0.f, cube_model);
		cube.set_model(cube_model);

		Matrix4f grids_model;
		get_Sim3(-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, grids_model);
		grids.set_model(grids_model);

		mtx.unlock();

		ang += 1.f;
		
		if (ang > 360) {
			ang = 0.f;
		}
		
		if(bdraw_cube)
			cube.draw();

		if (bdraw_light_cube)
			light_cube.draw();

		if (bdraw_wire_plane)
			wire_plane.draw();

		if (bdraw_grids)
			grids.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
		Sleep(100);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	t.join();

	return true;
}