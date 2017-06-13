#include <iostream>
#include <string>
#include <math.h>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace Eigen;
using namespace cv;

#include "util.h"
#include  "math.h"

void get_proj(const float fx, const float fy,
	const float cx, const float cy, Matrix4f &P) {
	P << fx, 0.f, cx, 0.f,
		0.f, fy, cy, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f;
}

//refer to http://www.songho.ca/opengl/gl_projectionmatrix.html
void get_frustum(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P) {
	float n2 = 2 * n;
	float irl = 1.f / (r - l);
	float itb = 1.f / (t - b);
	float ifn = 1.f / (f - n);

	P <<
		n2 * irl, 0.f, (r + l) * irl, 0.f,
		0.f, n2 * itb, (t + b) * itb, 0.f,
		0.f, 0.f, -(f + n) * ifn, -n2 * f * ifn,
		0.f, 0.f, -1.f, 0.f;
}

void get_ortho(const float l, const float r,
	const float b, const float t,
	const float n, const float f, Matrix4f &P) {
	const float irl = 1.f / (r - l);
	const float itb = 1.f / (t - b);
	const float ifn = 1.f / (f - n);
	P <<
		2.f * irl, 0.f, 0.f, -(r + l) * irl,
		0.f, 2.f * itb, 0.f, -(t + b) * itb,
		0.f, 0.f, -2.f * ifn, -(f + n) * ifn,
		0.f, 0.f, 0.f, 1.f;
}

void get_perspective(const float fov, const float aspect, const float near, const float far, Matrix4f &P) {
	float top = tan(fov / 2) * near;
	float bottom = -top;
	float right = top * aspect;
	float left = -right;
	get_frustum(left, right, bottom, top, near, far, P);
}

void get_Rx(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1;
}

void get_Ry(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		c, 0, -s, 0,
		0, 1, 0, 0,
		s, 0, c, 0,
		0, 0, 0, 1;
}

void get_Rz(const float ang, Matrix4f &R) {
	const float c = cosf(ang);
	const float s = sinf(ang);
	R <<
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
}


void get_Sim3(const float s, const float xang, const float yang, const float zang,
	const float x, const float y, const float z, Matrix4f &Sim3) {
	Matrix4f Rx, Ry, Rz;
	get_Rx(xang, Rx);
	get_Ry(yang, Ry);
	get_Rz(zang, Rz);

	float _s = 1.f + s;
	Matrix4f m = Rz * Ry * Rx;
	Sim3 <<
		_s * m(0, 0), m(0, 1), m(0, 2), x,
		m(1, 0), _s * m(1, 1), m(1, 2), y,
		m(2, 0), m(2, 1), _s * m(2, 2), z,
		0.f, 0.f, 0.f, 1.f;
}

void get_Sim3(const float s, const Matrix3f& R, const Vector3f& t, Matrix4f &Sim3) {
	float _s = 1.f + s;
	Sim3 <<
		_s * R(0, 0), R(0, 1), R(0, 2), t(0),
		R(1, 0), _s * R(1, 1), R(1, 2), t(1),
		R(2, 0), R(2, 1), _s * R(2, 2), t(2),
		0.f, 0.f, 0.f, 1.f;
}

void get_SE3_inv(const float xang, const float yang, const float zang, const float x, const float y, const float z, Matrix4f &SE3) {
	Matrix4f Rx, Ry, Rz;
	get_Rx(xang, Rx);
	get_Ry(yang, Ry);
	get_Rz(zang, Rz);

	Matrix4f m = Rz * Ry * Rx;
	const float _x = m(0, 0) * x + m(1, 0) * y + m(2, 0) * z;
	const float _y = m(0, 1) * x + m(1, 1) * y + m(2, 1) * z;
	const float _z = m(0, 2) * x + m(1, 2) * y + m(2, 2) * z;
	SE3 <<
		m(0, 0), m(1, 0), m(2, 0), -_x,
		m(0, 1), m(1, 1), m(2, 1), -_y,
		m(0, 2), m(1, 2), m(2, 2), -_z,
		0.f, 0.f, 0.f, 1.f;
}

void convert_rgb_to_hsv(const float r, const float g, const float b,
	float &h, float &s, float &v) {
	const float max_val = max(r, max(g, b));
	const float min_val = min(r, min(g, b));
	float c = max_val - min_val;

	h = 0.f;
	if (abs(c) < FLT_EPSILON) {
	}
	else if (max_val == r) {
		h = fmod((g - b) / c, 6.f);
	}
	else if (max_val == g) {
		h = (b - r) / c + 2;
	}
	else {
		h = (r - g) / c + 4;
	}

	h = 60 * h;
	if (h < 0)
		h += 360;

	v = max_val;

	if (abs(v) < FLT_EPSILON)
		s = 0.f;
	else
		s = c / v;
}

void convert_hsv_to_rgb(const float h, const float s, const float v,
	float &r, float &g, float &b) {
	const float c = s * v;
	const float _h = h / 60.f;
	const float x = c * (1 - abs(fmod(_h, 2.f) - 1.f));
	if (is_range(0.f, 1.f, _h)) {
		r = c;
		g = x;
		b = 0.f;
	}
	else if (is_range(1.f, 2.f, _h)) {
		r = x;
		g = c;
		b = 0.f;
	}
	else if (is_range(2.f, 3.f, _h)) {
		r = 0.f;
		g = c;
		b = x;
	}
	else if (is_range(3.f, 4.f, _h)) {
		r = 0.f;
		g = x;
		b = c;
	}
	else if (is_range(4.f, 5.f, _h)) {
		r = x;
		g = 0.f;
		b = c;
	}
	else if (is_range(5.f, 6.f, _h)) {
		r = c;
		g = 0.f;
		b = x;
	}
	const float m = v - c;
	r += m;
	g += m;
	b += m;
}

float adjustment(const float color, const float  factor, const float gamma) {
	if (abs(color) < FLT_EPSILON) {
		return 0.f;
	}
	else {
		return powf(color * factor, gamma);
	}
}

void convert_wavelength_to_rgb(const float gamma,
	const float wl, float &r, float &g, float &b) {
	if (is_range(380.f, 440.f, wl)) {
		r = (440.f - wl) / (440.f - 380.f);
		g = 0.f;
		b = 1.f;
	}
	else if (is_range(440.f, 490.f, wl)) {
		r = 0.f;
		g = (wl - 440.f) / (490.f - 440.f);
		b = 1.f;
	}
	else if (is_range(490.f, 510.f, wl)) {
		r = 0.f;
		g = 1.f;
		b = (510.f - wl) / (510.f - 490.f);
	}
	else if (is_range(510.f, 580.f, wl)) {
		r = (wl - 510.f) / (580.f - 510.f);
		g = 1.f;
		b = 0.f;
	}
	else if (is_range(580.f, 645.f, wl)) {
		r = 1.f;
		g = (645.f - wl) / (645.f - 580.f);
		b = 0.f;
	}
	else if (is_range(645.f, 780.f, wl)) {
		r = 1.f;
		g = 0.f;
		b = 0.f;
	}
	else {
		r = g = b = 0.f;
	}

	float factor;
	if (is_range(380.f, 420.f, wl)) {
		factor = 0.3f + 0.7f * (wl - 380.f) / (420.f - 380.f);
	}
	else if (is_range(420.f, 700.f, wl)) {
		factor = 1.f;
	}
	else if (is_range(700, 780, wl)) {
		factor = 0.3f + 0.7f * (780.f - wl) / (780.f - 700.f);
	}
	else {
		factor = 0.f;
	}

	r = adjustment(r, factor, gamma);
	g = adjustment(g, factor, gamma);
	r = adjustment(b, factor, gamma);
}

float get_rand(const float max_val, const float min_val) {
	static default_random_engine generator(static_cast<uint>(time(NULL)));
	uniform_real_distribution<float> distribution(0.f, 1.f);
	return distribution(generator);
	
}