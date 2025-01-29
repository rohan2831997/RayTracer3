#ifndef HITTABLE_H
#define HITTABLE_H

#include"ray.h"
#include"rtweekend.h"
#include "aabb.h"
#include <cmath>

class material;

class hit_record {
public:
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
	double u;	// For textures
	double v;	// For textures
	bool front_face;

	void set_face_normal(const ray& r, const vec3& outward_normal) {
		//sets the hit record normal vector.
		//NOTE: outward_normal is assumed to have unit length.

		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};



class hittable
{
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r,
					 interval ray_t,
					 hit_record& rec) const = 0;
	virtual aabb bounding_box() const = 0;
	
};

class translate : public hittable {
private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
public:
	translate(shared_ptr<hittable> p, const vec3& displacement)
		: object(p), offset(displacement)
	{
		bbox = object->bounding_box() + offset;
	}
	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		//move the ray backwards by the offset
		ray offset_r(r.origin() - offset, r.direction(), r.time());

		// Determine where (if any) intersection occurs along the offset ray
		if (!object->hit(offset_r, ray_t, rec))
			return false;

		//Move the intersection point forwards by an offset
		rec.p += offset;
		return true;
	}

	aabb bounding_box() const {
		return bbox;
	}
};

class rotate_y : public hittable {

private:
	shared_ptr<hittable> object;
	double sin_theta;
	double cos_theta;
	aabb bbox;
public:

	rotate_y(shared_ptr<hittable> p, double angle) : object(p) {
		auto radians = degrees_to_radians(angle);
		sin_theta = sin(radians);
		cos_theta = cos(radians);
		bbox = object->bounding_box();

		point3 min(infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

					auto newx = cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++)
					{
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		// change the ray from world space to object space
		auto origin = r.origin();
		auto direction = r.direction();

		origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
		origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

		direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
		direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

		ray rotated_r(origin, direction, r.time());

		// determine where (if any) an intersection occurs in object space
		if (!object->hit(rotated_r, ray_t, rec))
			return false;

		// change intersection point from object space to world space
		auto p = rec.p;
		p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
		p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

		//change the normal
		auto normal = rec.normal;
		normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
		normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

		rec.p = p;
		rec.normal = normal;

		return true;
	}

	aabb bounding_box() const override { return bbox; }
};

#endif // !HITTABLE_H