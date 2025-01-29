#ifndef RAY_H
#define RAY_H
#include"vec3.h"

class ray {
private:
	point3 orig;
	vec3 dir;
	double tm;

public:
	ray(){}
	ray(const point3& orig, const vec3& direc)						: orig(orig), dir(direc), tm(0) {}
	ray(const point3& orig, const vec3& direc, double time = 0.0)	: orig(orig), dir(direc), tm(time) {}

	point3 origin() const	{ return orig; };
	vec3 direction() const	{ return dir; };
	double time() const		{ return tm; }

	point3 at(double t) const
	{
		return orig + (t * dir);
	};

};

#endif // !RAY_H
