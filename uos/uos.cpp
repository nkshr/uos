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
#include "drawing_component.h"
#include "uos.h"

c_uos::c_uos() {
}

c_uos::~c_uos() {

}

void c_uos::draw() {
	cube.draw();
	wire_cube.draw();
	light_cube.draw();
	wire_plane.draw();
}



bool c_uos::init() {
	if (!cube.init())
		return false;
	if (!wire_cube.init())
		return false;
	if (!light_cube.init())
		return false;
	if (!wire_plane.init())
		return false;

	return true;
}

void c_uos::destroy() {
}

void c_uos::set_proj(const Matrix4f &proj) {
	c_draw_comp::set_proj(proj);
}

void c_uos::set_view(const Matrix4f &view) {
	c_draw_comp::set_view(view);
}

void c_uos::set_cube_model(const  Matrix4f &model) {
	cube.set_model(model);
	wire_cube.set_model(model);
}

void c_uos::set_light(const s_Phong_light &light) {
	c_draw_comp::set_light(light);
}