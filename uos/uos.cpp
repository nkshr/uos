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

uos::uos() {
}

uos::~uos() {

}

void uos::draw() {
	c_draw_comp::set_light(light);
	//cube.draw();
	wire_cube.draw();
	light_cube.draw();
	wire_plane.draw();
}



bool uos::init() {
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

void uos::destroy() {
}

void uos::set_proj(const Matrix4f &proj) {
	c_draw_comp::set_proj(proj);
}

void uos::set_view(const Matrix4f &view) {
	c_draw_comp::set_view(view);
}

void uos::set_cube_model(const  Matrix4f &model) {
	cube.set_model(model);
	wire_cube.set_model(model);
}

void uos::set_light_pos(const Vector3f &light_pos) {
	light.pos = light_pos;
}

void uos::set_light_col(const Vector3f &light_col) {
	light.col = light_col;
}

void uos::set_amb_light_pwr(const Vector3f &amb_light_pwr) {
	light.amb_light_pwr = amb_light_pwr;
}

void uos::set_spec_light_col(const Vector3f &spec_light_col) {
	light.spec_light_col = spec_light_col;
}

void uos::set_light_pwr(const float light_pwr) {
	light.pwr = light_pwr;
}

