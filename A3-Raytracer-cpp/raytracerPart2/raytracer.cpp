/***********************************************************
	
	Starter code for Assignment 3

	Implementations of functions in raytracer.h, 
	and the main function which specifies the scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <random>
#include <chrono>

//you can set true or false to turn these effects on or off
#define samples 4
#define antialiasing true
#define softshadow true
#define hardshadow false
#define DOF true
#define DOF_focal_length 6
#define DOF_APERTURE_RADIUS 0.8
#define roughness 0.2
#define glossy true
#define recursiveRay true
#define texture_on true

//reference on http://www.cplusplus.com/reference/random/normal_distribution/normal_distirbution/
int normal_distribution(){
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::normal_distribution<double> distribution(0.0,1.0);
	int ranNum = distribution(generator);
	return ranNum < 0 ? -ranNum : ranNum;
}

void Raytracer::traverseScene(Scene& scene, Ray3D& ray)  {
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

		if (node->obj->intersect(ray, node->worldToModel, node->modelToWorld)) {
			ray.intersection.mat = node->mat;
		}
	}
}

void Raytracer::computeTransforms(Scene& scene) {
	// right now this method might seem redundant. But if you decide to implement 
	// scene graph this is where you would propagate transformations to child nodes
		
	for (size_t i = 0; i < scene.size(); ++i) {
		SceneNode* node = scene[i];

		node->modelToWorld = node->trans;
		node->worldToModel = node->invtrans; 
	}
}

void Raytracer::computeShading(Ray3D& ray, LightList& light_list, Scene& scene) {
	for (size_t  i = 0; i < light_list.size(); ++i) {
		LightSource* light = light_list[i];
		
		//Each lightSource provides its own shading function.
		//Implement shadows here if needed.
		//create ray from intersection to light

		// reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
		// Shadows
		//HARD SHADOW
		if(hardshadow){
			Ray3D shadowRay;
			shadowRay.dir  = light->get_position() - ray.intersection.point;
			shadowRay.dir.normalize();
			shadowRay.origin = ray.intersection.point + 0.001 * shadowRay.dir;
			traverseScene(scene, shadowRay);
			if(shadowRay.intersection.none){
				light->shade(ray);        
			}
		}
		// reference on tutorial slide Ray Tracing  - Part 2 from TA Muhammed Anwar
		// Soft Shadows - Method 2
		//SOFT SHADOW
		if(softshadow){
			for(unsigned i = 0; i < 3; i++){

				//***********random number version*****************
				double r = 3.0 * (rand()%100/100.0);
				double theta = 2.0 * M_PI * (rand()%100/100.0);
				double phi = 2.0 * M_PI * (rand()%100/100.0);

				Point3D randomLightPos = light->get_position() + Vector3D(r*cos(theta)*sin(phi),r*sin(theta)*sin(phi),r*cos(phi));

				//***********normal distribution version************
				// double r = 3.0 * normal_distribution();
				// double theta = 2.0 * M_PI * normal_distribution();
				// double phi = 2.0 * M_PI * normal_distribution();

				// Point3D randomLightPos = light->get_position() + Vector3D(r*cos(theta)*sin(phi),r*sin(theta)*sin(phi),r*cos(phi));

				Ray3D shadowRay;
				shadowRay.dir  = randomLightPos - ray.intersection.point;
				shadowRay.dir.normalize();
				shadowRay.origin = ray.intersection.point + 0.001 * shadowRay.dir;
				shadowRay.col = Color(0,0,0);
				//ray.intersection.point = Point3D((rand()%10/10),(rand()%10/10),0);
				traverseScene(scene, shadowRay);
				if(shadowRay.intersection.none){
					light->shade(ray);        
				}
			}
		}

		//without shadow
		if(!softshadow && !hardshadow){
			light->shade(ray);
		}
	}
}

Color Raytracer::shadeRay(Ray3D& ray, Scene& scene, LightList& light_list, int depth) {
	Color col(0.0, 0.0, 0.0); 
	traverseScene(scene, ray); 

	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {

		computeShading(ray, light_list, scene); 
		col = col + ray.col;

		// Recursive ray tracing --> reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
		// convert from pseudocode of ray shading
		if(depth > 0){
			//create a reflected ray at the intersection
			Color reflectCol(0.0,0.0,0.0);
			Ray3D reflectRay;
			Vector3D reflectDir = ray.dir - (2.0 * (ray.dir.dot(ray.intersection.normal)) * ray.intersection.normal);
			reflectDir.normalize();
			Point3D reflectOrigin = ray.intersection.point;

			reflectRay.dir = reflectDir;
			reflectRay.dir.normalize();
			//prevent reflected in same point again
			reflectRay.origin = reflectOrigin + 0.001 * reflectDir;


			// reference on tutorial slide Ray Tracing  - Part 2 from TA Muhammed Anwar
			// Glossy Reflections - Jitter pseudocode 
			// glossy reflection
			// orthonormal basis at intersection point
			if(glossy && ray.intersection.mat->isGlossy){
				Vector3D u = reflectDir.cross(ray.intersection.normal);
				u.normalize();
				Vector3D v = reflectDir.cross(u);
				v.normalize();

				//choose uniformly sampled random direction to send the ray in
				double theta = 2 * M_PI * (((rand()%100)/100.0) * roughness);
				double phi = 2 * M_PI * (((rand()%100)/100.0) * roughness);
				double x = sin(theta) * cos(phi);
				double y = sin(theta) * sin(phi);
				double z = cos(theta);

				reflectDir = x * u + y * v + z * reflectDir;
				reflectDir.normalize();
				reflectRay.dir = reflectDir;
			}

			//recursive ray tracing
			if(recursiveRay){
				//col = col + shadeRay(reflectRay, scene, light_list, --depth);
				shadeRay(reflectRay, scene, light_list, --depth);
				//decrease the reflectedRay color 
				col = col + 0.5 * reflectRay.col;
			}
		}

		// Texture mapping
		if(texture_on){
			//a null pointer found here to prevent segfault
            if(ray.intersection.mat->texture != NULL){
                col = col + ray.intersection.mat->texture->getColor(ray);
            }
		}
	}

	// You'll want to call shadeRay recursively (with a different ray, 
	// of course) here to implement reflection/refraction effects.  
	col.clamp();
	return col; 
}	

void Raytracer::render(Camera& camera, Scene& scene, LightList& light_list, Image& image, int depth) {
	computeTransforms(scene);

	Matrix4x4 viewToWorld;
	double factor = (double(image.height)/2)/tan(camera.fov*M_PI/360.0);

	viewToWorld = camera.initInvViewMatrix();

	// Construct a ray for each pixel.
	for (int i = 0; i < image.height; i++) {
		for (int j = 0; j < image.width; j++) {

			// supersampling from https://github.com/trentmwillis/ray-tracer/blob/master/main.c
			// antialiasing 
			// Sets up ray origin and direction in view space, 
			// image plane is at z = -1.
			if(antialiasing){
				Color col(0.0,0.0,0.0);
				for(int p = 0; p < samples; p++){
					for(int q = 0; q < samples; q++){
						double r =  ((rand() % 100)/100.0);
						double x = (p + r)/samples;
						double y = (q + r)/samples;

						Point3D origin(0.0, 0.0, 0.0);
						Point3D imagePlane;
						imagePlane[0] = (-double(image.width)/2 + 0.5 + j + x)/factor;
						imagePlane[1] = (-double(image.height)/2 + 0.5 + i + y)/factor;
						imagePlane[2] = -1;

						// reference on tutorial slide Ray Tracing  - Part 2 from TA Muhammed Anwar
						// Depth-of-Field formula 
						// **************DOF**************
						// start to implement depth of field with samples supported from antialiasing
						if(DOF){

							//primary ray =  o + tD
							Vector3D primaryRay = imagePlane - origin;
							primaryRay.normalize();

							//t_value of a plane -> t = - -imagePlane_Z / rayDirection_Z
							double t_value =  -( -imagePlane[2] / primaryRay[2]);

							//Focal point = o + F*t*D
							Point3D focal_point = origin + DOF_focal_length * t_value * primaryRay;

							//random point on lens = o + r 
							double angle  = 2 * M_PI * ((rand() % 100)/100.0);
							double radius = DOF_APERTURE_RADIUS * ((rand() % 100)/100.0);
							Point3D ranLenPoint = Point3D(radius*cos(angle),radius*sin(angle),0);

							//secondary ray direction  = c - (o + r)
							Vector3D secondaryRayDir = focal_point - ranLenPoint;
							secondaryRayDir = viewToWorld * secondaryRayDir;
							secondaryRayDir.normalize();

							//secondary ray  = (o+r) + t(c - (o + r))
							ranLenPoint = viewToWorld * ranLenPoint;
							Ray3D secondaryRay(ranLenPoint,secondaryRayDir);

							col = col + (1.0/(samples*samples)) * shadeRay(secondaryRay, scene, light_list, depth);

						}else{
							// reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
							// Ray Casting Pseudo-Code Part
							// convert camera coordinate into world coordinate
							Vector3D viewCamera = imagePlane - origin;
							Vector3D viewWorld = viewToWorld * viewCamera;
							Point3D originWorld = viewToWorld * origin;

							//this is a ray sent from a world point
							Ray3D ray(originWorld, viewWorld);
							// TODO: Convert ray to world space 
							
							col = col + (1.0/(samples*samples)) * shadeRay(ray, scene, light_list, depth);
						}
						// col[0] = col[0] / (samples * samples);
						// col[1] = col[1] / (samples * samples);
						// col[2] = col[2] / (samples * samples);
						col.clamp();
						//Color col = shadeRay(ray, scene, light_list,depth);
						image.setColorAtPixel(i, j, col);		
					}
				}	
		}else{

					Point3D origin(0.0, 0.0, 0.0);
					Point3D imagePlane;
					imagePlane[0] = (-double(image.width)/2 + 0.5 + j)/factor;
					imagePlane[1] = (-double(image.height)/2 + 0.5 + i)/factor;
					imagePlane[2] = -1;

					// reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
					// Ray Casting Pseudo-Code Part
					// convert camera coordinate into world coordinate
					Vector3D viewCamera = imagePlane - origin;
					Vector3D viewWorld = viewToWorld * viewCamera;
					Point3D originWorld = viewToWorld * origin;

					//this is a ray sent from a world point
					Ray3D ray(originWorld, viewWorld);
					Color col = shadeRay(ray, scene, light_list,depth);
					image.setColorAtPixel(i, j, col);		
			}
		}
	}
}

