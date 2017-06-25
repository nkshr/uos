#pragma once

struct s_obj;

struct s_material {
	Vector3f emittance;
	Vector3f reflectance;
};

struct s_ray {
	Vector3f orig;
	Vector3f dir;

	s_obj *obj_hit;

	void find_nearest_obj_hit();
	Vector3f pt_where_obj_was_hit;
	Vector3f normal_where_obj_was_hit;
};

struct s_prim {
	Vector3f col;
	Vector3f emittance;
	Vector3f reflectance;;
	Vector3f *v;
	Vector3f normal;
	Vector3f cg;
	
	bool hit;

	int num_vertices;

	float dist;

	s_prim() {};
	s_prim(const s_prim &prim) {
		emittance = prim.emittance;
		reflectance = prim.reflectance;

		num_vertices = 3;
		v = new Vector3f[num_vertices];
		for (int i = 0; i < num_vertices; ++i)
			v[i] = prim.v[i];

		hit = prim.hit;
		normal = prim.normal;
		cg = prim.cg;
		dist = prim.dist;
	}

	void calc_dist() {
		dist = cg.norm();
	}

	virtual void init() {};
	virtual void calc_normal() {};
	virtual void calc_center_of_gravity() {};
	virtual void check_hit(const s_ray &ray) {};
};

struct s_obj {
	vector<s_prim*> prims;
	s_prim *prim_hit;
	void find_prim_hit(const s_ray &ray) {
		vector<s_prim*> prims_hit;
		for (vector<s_prim*>::iterator it = prims.begin(); it != prims.end(); ++it) {
			(*it)->check_hit(ray);
			if ((*it)->hit) {
				prims_hit.push_back(*it);
			}
			else {
			
			}
		}

		if (!prims_hit.size()) {
			prim_hit = NULL;
		}

		float dist = FLT_MAX;
		for (vector<s_prim*>::iterator it_prims = prims.begin();
			it_prims != prims.end(); ++it_prims) {
			if (dist > (*it_prims)->dist) {
				prim_hit = *it_prims;
			}
		}
	}	

	float* gen_poss() {
		float *ret = new float[prims.size * prims[0]->num_vertices * 3];
		for (vector<s_prim*>::iterator it_prims = prims.begin();
			it_prims != prims.end(); ++it_prims) {
			for (int i = 0; i < (*it_prims)->num_vertices; ++i) {
				*ret = (*it_prims)->v[i][0];
				++ret;
				*ret = (*it_prims)->v[i][1];
				++ret;
				*ret = (*it_prims)->v[i][2];
				++ret;
			}
		}
	}

	float* gen_cols() {
		float *ret = new float[prims.size * 3];
		for (vector<s_prim*>::iterator it_prims = prims.begin();
			it_prims != prims.end(); ++it_prims) {
			*ret = (*it_prims)->col[0];
			++ret;
			*ret = (*it_prims)->col[1];
			++ret;
			*ret = (*it_prims)->col[2];
			++ret;
		}
	}

	float* gen_normals() {
		float *ret = new float[prims.size * 3];
		for (vector<s_prim*>::iterator it_prims = prims.begin();
			it_prims != prims.end(); ++it_prims) {
			*ret = (*it_prims)->normal[0];
			++ret;
			*ret = (*it_prims)->normal[1];
			++ret;
			*ret = (*it_prims)->normal[2];
			++ret;
		}
	}
};

static vector<s_obj*> objs;

struct s_triangle: public s_prim {
	s_triangle() {
		num_vertices = 3;
		v = new Vector3f[num_vertices];
	}

	virtual void calc_center_of_gravity() {
		cg = (v[0] + v[1] + v[2]) / 3.f;
	}

	//I assume clockwise.
	virtual void calc_normal() {
		Vector3f v0v1 = v[1] - v[0];
		Vector3f v0v2 = v[2] - v[0];
		normal = v0v2.cross(v0v1);
		normal.normalize();
	}

	virtual void check_intersec(const s_ray &ray) {
		float normal_dot_ray_dir = normal.dot(ray.dir);
		if (fabs(normal_dot_ray_dir) < FLT_EPSILON) {
			hit = false;
			return;
		}

		float d = normal.dot(v[0]);
		float t = (normal.dot(ray.orig) + d) / normal.dot(ray.dir);

		if (t < 0) {
			hit = false;
			return;
		}

		Vector3f p = ray.orig + t  * ray.dir;
		Vector3f edge = v[1]- v[0];
		Vector3f vp = p - v[0];
		Vector3f c = vp.cross(edge);
		if (normal.dot(c) < 0) {
			hit = false;
			return;
		}

		edge = v[2] - v[1];
		vp = p - v[1];
		c = vp.cross(edge);
		if (normal.dot(c) < 0) {
			hit = false;
			return;
		}

		edge = v[0] - v[2];
		vp = p - v[2];
		c = vp.cross(edge);
		if (normal.dot(c) < 0) {
			hit = false;
			return;
		}

		hit = true;
	}

	~s_triangle() {
		delete v;
	}
};

Vector3f random_unit_vector_in_hemisphere_of(const Vector3f &normal);