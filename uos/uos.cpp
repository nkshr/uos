#include <vector>
#include <iterator>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;

#include "uos.h"

#define BLACK Vector3f(1.f, 1.f, 1.f)
#define TRIANGLE 0

const int max_depth = 10;

void s_ray::find_nearest_obj_hit() {
	vector<s_obj*> objs_hit;
	for (vector<s_obj*>::iterator it_objs = objs.begin(); it_objs != objs.end(); ++it_objs) {
		(*it_objs)->find_prim_hit(*this);
		if ((*it_objs)->prim_hit) {
			objs_hit.push_back(*it_objs);
		}
	}

	obj_hit = NULL;
	float dist = FLT_MAX;
	for (vector<s_obj*>::iterator it_objs = objs_hit.begin(); it_objs != objs.end(); ++it_objs) {
		if (dist > (*it_objs)->prim_hit->dist) {
			obj_hit = *it_objs;
		}
	}
}

Vector3f trace_path(s_ray ray, const int depth) {
	if (depth == max_depth) {
		return BLACK;
	}

	ray.find_nearest_obj_hit();
	if (!ray.obj_hit) {
		return BLACK;
	}
	
	s_ray new_ray;
	new_ray.orig = ray.pt_where_obj_was_hit;
	new_ray.dir = random_unit_vector_in_hemisphere_of(ray.normal_where_obj_was_hit);

	float cos_theta = new_ray.dir.dot(ray.normal_where_obj_was_hit);
	s_prim *prim_hit = ray.obj_hit->prim_hit;
	Vector3f BRDF = 2 * cos_theta * prim_hit->reflectance;

	Vector3f reflected = trace_path(new_ray, depth + 1);

	return prim_hit->emittance + BRDF.cwiseProduct(reflected);
}

Vector3f random_unit_vector_in_hemisphere_of(const Vector3f &normal) {
	return Vector3f(1.f, 1.f, 1.f);
}