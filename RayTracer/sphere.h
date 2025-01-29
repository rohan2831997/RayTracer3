#ifndef SPHERE_H
#define SPHERE_H

#include"hittable.h"
#include"vec3.h"

class sphere : public hittable
{
private:
	point3 centre1;
    vec3 centre_vec;
	double radius;
    shared_ptr<material> mat;
    bool is_moving;
    aabb bbox;

    point3 sphere_centre(double time) const {
        // Linear Interpolate from centre1 to cemtre2

        return centre1 + time * centre_vec;
    }

    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p : point on the sphere of radius 1, centred at origin
        // u : returned values[0,1] of angle from 
        // v: returned values [0,1] of angle from y=-1 to y=+1

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

public:
    //Stationary Sphere
	sphere(point3 _centre, double _radius, shared_ptr<material> _material) : centre1(_centre), 
                                                            radius(_radius), mat(_material),
                                                            is_moving(false)                
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(centre1 - rvec, centre1 + rvec);
    };

    //Moving sphere
    sphere(point3 _centre1, point3 _center2, double _radius, shared_ptr<material> _material) : centre1(_centre1),
        radius(_radius), mat(_material),
        is_moving(true) 
    {
        auto rvec = vec3(radius, radius, radius);
        aabb box1 = aabb(_centre1 - rvec, _centre1 + rvec);
        aabb box2 = aabb(_center2 - rvec, _center2 + rvec);
        bbox = aabb(box1, box2);

        centre_vec = _center2 - centre1;
    };

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        point3 centre = is_moving ? sphere_centre(r.time()) : centre1;
        vec3 oc = r.origin() - centre;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant < 0) {
            return false;
        }
        
        auto sqrtd = sqrt(discriminant);
        // Find the nearest root that lies in the acceptable range
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - centre) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;

	}

    aabb bounding_box() const override { return bbox; }
};


#endif // !SPHERE_H
