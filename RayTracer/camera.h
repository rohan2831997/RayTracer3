#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h";

#include "color.h"
#include "hittable.h"
#include "material.h"

void write_color(std::ostream& out, const color& pixel_color, int samples_pex_pixel)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	auto scale = 1.0 / samples_pex_pixel;
	r *= scale;
	g *= scale;
	b *= scale;

	// Apply the linear to gamma transform
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// Write the translated [0,255] value of each color component.
	static const interval intensity(0.000, 0.999);
	out << static_cast<int>(255.999 * intensity.clamp(r)) << ' '
		<< static_cast<int>(255.999 * intensity.clamp(g)) << ' '
		<< static_cast<int>(255.999 * intensity.clamp(b)) << '\n';
}

class camera
{
public:
	double aspect_ratio = 1.0; // Ratio of image width over height
	int	   image_width  = 100; // Rendered image width in pixel count
	int    samples_per_pixel = 10;
	int    max_depth = 10;
	color  background;

	double vfov = 90; // Vertical view angle( field of view)
	point3 lookfrom = point3(0, 0, -1);
	point3 lookat = point3(0, 0, 0);
	vec3 vup = vec3(0, 1, 0);

	double defocus_angle = 0;	// Variation angle
	double focus_dist = 10;		// Imagine a cone with centre at the ficus plane

	void render(hittable& world)
	{
		initialize();

		// Render

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (size_t j = 0; j < image_height; ++j)
		{
			for (size_t i = 0; i < image_width; i++)
			{
				//to the log
				std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

				color pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; ++sample) {
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}
				//std::cout << ir << ' ' << ig << ' ' << ib << '\n';
				write_color(std::cout, pixel_color,samples_per_pixel);
			}
		}
		std::clog << "\rDone.                 \n";
	};
	// go through the website
	// hardware
	// VUlkan
	// Maths
private:
	int image_height;	// Rendered image height
	point3 center;		// Camera Centre
	point3 pixel00_loc;	// Location of pixel 0, 0
	vec3 pixel_delta_u;	// x offset
	vec3 pixel_delta_v;	// y offset
	vec3 u, v, w;
	vec3 defocus_disk_u;
	vec3 defocus_disk_v;

	void initialize()
	{
		// Calculate the image height, and ensure that it's at least 1.
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		center = lookfrom;

		// Viewport Dimensions
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0* h*focus_dist;
		auto viewport_width = viewport_height * static_cast<double>(image_width) / image_height;

		// Make u,v,w the basis vectors relative to the camera coordinate space
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		//Calculate the vectors along the view port edges to determine pixel colors
		vec3 viewport_u = viewport_width * u;
		vec3 viewport_v = viewport_height * -v;

		//Calculating the pixel delta coordiantes
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		//Calculating the value of the upper left pixel
		point3 viewport_upper_left = center +
			(-focus_dist * w)+
			(-viewport_u) / 2 +
			(-viewport_v) / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate the camera defocus basis vectors
		auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2.0));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	};

	color ray_color(const ray& r, int depth, const hittable& world) const {
		hit_record rec;

		if (depth <= 0)
			return color(0, 0, 0);

		if (world.hit(r, interval(0.0001, infinity), rec)) //0.001 to fix shadow acne 
		{
			ray scattered;
			color attenuation;
			color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

			if (!rec.mat->scatter(r, rec, attenuation, scattered))
				return color_from_emission;

				color color_from_scatter = attenuation * ray_color(scattered, depth-1, world);


				return color_from_emission + color_from_scatter;
		}

		else
		{
			return background;
		}

		// if the ray hits nothing just return the background color
		
	}

	ray get_ray(int i, int j) const {
		point3 pixel_centre = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		auto pixel_sample = pixel_centre + pixel_sample_square();

		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;
		auto ray_time = random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	point3 defocus_disk_sample() const {
		// It will return a random point in the camera defocus disk
		auto p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	point3 pixel_sample_square() const {
		//Returns a random point surrounding the original point
		auto px = random_double(-0.5, +0.5);
		auto py = random_double(-0.5, +0.5);
		return (px * pixel_delta_u) + (py * pixel_delta_v);
	}
};



#endif