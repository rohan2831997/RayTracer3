#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
public:
	double e[3];

	vec3() : e{ 0,0,0 } {}
	vec3(double x, double y, double z) : e{ x,y,z } {};

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator -() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	double x() { return e[0]; }
	double y() { return e[1]; }
	double z() { return e[2]; }

	vec3& operator +=(const vec3 v)
	{
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];

		return *this;
	}

	vec3& operator *=(double d)
	{
		e[0] *= d;
		e[1] *= d;
		e[2] *= d;

		return *this;
	}

	vec3& operator/=(double t) {
		return *this *= 1 / t;
	}

	double length_squared() const
	{
		return e[0] * e[0] +
			e[1] * e[1] +
			e[2] * e[2];
	}

	double length() const
	{
		return sqrt(this->length_squared());
	}

	static double random_double() {
		// Returns a random real in [0,1)
		return rand() / (RAND_MAX + 1.0);
	}

	static double random_double(double min, double max) {
		return min + (max - min) * rand() / (RAND_MAX + 1.0);
	}

	static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in all dimensions.
		auto s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
	}
};
	//Using point3 as an alias for vec3, it is just easier in code
	using point3 = vec3;
	
	//vector utility functions
	inline std::ostream& operator<<(std::ostream& out, const vec3& v)
	{
		return out << v.e[0] << ' ' << v.e[1] <<' '<< v.e[2];
	}
		
	inline vec3 operator+(const vec3& u, const vec3& v) {
		return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
	}

	inline vec3 operator-(const vec3& u, const vec3& v) {
		return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
	}

	inline vec3 operator*(const vec3& u, const vec3& v) {
		return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	}

	inline vec3 operator*(double t, const vec3& v) {
		return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
	}

	inline vec3 operator*(const vec3& v, double t) {
		return t * v;
	}

	inline vec3 operator/(vec3 v, double t) {
		return (1 / t) * v;
	}

	inline double dot(const vec3& u, const vec3& v) {
		return u.e[0] * v.e[0]
			+ u.e[1] * v.e[1]
			+ u.e[2] * v.e[2];
	}

	inline vec3 cross(const vec3& u, const vec3& v) {
		return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
			u.e[2] * v.e[0] - u.e[0] * v.e[2],
			u.e[0] * v.e[1] - u.e[1] * v.e[0]);
	}

	inline vec3 unit_vector(vec3 v) {
		return v / v.length();
	}

	inline vec3 random_in_unit_sphere() {
		while (true) {
			auto p = vec3::random(-1, 1);
			if (p.length_squared() < 1)
				return p;
		}
	}

	inline vec3 random_unit_vector() {
		return unit_vector(random_in_unit_sphere());
	}

	inline vec3 random_on_hemisphere(const vec3& normal) {
		vec3 on_unit_sphere = random_unit_vector();
		if (dot(on_unit_sphere, normal) > 0.0)
			return on_unit_sphere;
		else
			return -on_unit_sphere;
	}

	inline vec3 reflect(const vec3& v, const vec3& _normal) {
		return v - 2 * dot(v, _normal) * _normal;
	}

	inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
		auto cos_theta = fmin(dot(-uv, n), 1.0);
		vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
		vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
		return r_out_perp + r_out_parallel;
	}

	inline vec3 random_in_unit_disk() {
		double min = -1;
		double max = +1;
		while (true) {
			double x = min + (max - min) * rand() / (RAND_MAX + 1.0);
			double y = min + (max - min) * rand() / (RAND_MAX + 1.0);
			auto p = vec3(x, y, 0);
			if (p.length_squared() < 1)
				return p;
		}
	}

#endif // !VEC3_H



