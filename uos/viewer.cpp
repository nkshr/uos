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
	//glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

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
	//glCullFace(GL_FRONT);

	uos simulator;
	Matrix4f proj;
	get_frustum(-640.f, 640.f, -480.f, 480.f, 1000.f, 10000.f, proj);
	simulator.set_proj(proj);
	cout << "proj : " << proj << endl;

	Vector3f light_pos;
	light_pos << 0.f, 500.f, 500.f;
	simulator.set_light_position(light_pos);

	Vector3f light_col;
	light_col << 1.f, 1.f, 1.f;
	simulator.set_light_color(light_col);

	//simulator.set_ambient_light_power(0.1f, 0.1f, 0.1f);
	//simulator.set_ambient_light_power(1.f, 1.f, 1.f);
	//simulator.set_ambient_light_power(0.f, 0.f, 0.f);
	simulator.set_specular_color(0.3f, 0.3f, 0.3f);
	//simulator.set_specular_color(1.f, 1.f, 1.f);
	simulator.set_light_power(200000);

	Mat texture = imread(texture_name);
	if (texture.empty()) {
		cerr << "Error : Couldn't read " << texture_name << "." << endl;
		return false;
	}


	if (!simulator.init())
		return false;

	float ang = 0.f;
	while (!glfwWindowShouldClose(window)) {
		int frame_width, frame_height;
		glfwGetFramebufferSize(window, &frame_width, &frame_height);
		glViewport(0, 0, frame_width, frame_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 0.f);

		Matrix4f view;
		get_SE3_inv(deg_to_rad(0), deg_to_rad(0), deg_to_rad(0), 0.f, 0.f, 3000, view);
		simulator.set_view(view);

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