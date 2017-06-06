#include <Windows.h>

#include <iostream>
#include <thread>
#include <mutex>


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
#include "uos.h"

const int win_width = 1280;
const int win_height = 960;
const char * texture_name = "golddiag.jpg";
const float trans_step = 100.f;
const float rot_step = 1.f;
observer obsv;

static void key_callback(GLFWwindow *wiindow, int key, int scancode, int action, int mods) {
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
		default:
			break;
		}
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

	uos simulator;
	if (!simulator.init())
		return false;
	Vector3f light_pos(500.f, 0.f, 1500.f);
	simulator.set_light_pos(light_pos);
	
	Vector3f light_col(1.f, 1.f, 1.f);
	simulator.set_light_col(light_col);
	
	simulator.set_light_pwr(10000000);

	Vector3f amb_light_pwr(0.5f, 0.5f, 0.5f);
	simulator.set_amb_light_pwr(amb_light_pwr);

	Vector3f spec_col(0.3f, 0.3f, 0.3f);
	simulator.set_spec_light_col(spec_col);

	Mat texture = imread(texture_name);
	if (texture.empty()) {
		cerr << "Error : Couldn't read " << texture_name << "." << endl;
		return false;
	}

	obsv.center = Vector3f(0.f, 0.f, 0.f);
	obsv.up = Vector3f(0.f, 1.f, 0.f);

	get_perspective(deg_to_rad(60.f), 640.f/ 480.f, 1000, 4500, obsv.proj);
	simulator.set_proj(obsv.proj);

	obsv.eye = Vector3f(0.f, 0.f, 3000.f);
	obsv.update();
	float ang = 0.f;
	while (!glfwWindowShouldClose(window)) {
		int frame_width, frame_height;
		glfwGetFramebufferSize(window, &frame_width, &frame_height);
		glViewport(0, 0, frame_width, frame_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 0.f);

		simulator.set_view(obsv.view);

		Matrix4f cube_model;
		get_Sim3(1.f, deg_to_rad(ang), deg_to_rad(0), 0.f, 0.f, 0.f, 0.f, cube_model);
		simulator.set_cube_model(cube_model);
		ang += 1.f;
		
		if (ang > 360) {
			ang = 0.f;
		}

		simulator.draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
		Sleep(100);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}