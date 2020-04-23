/***********************************************************
	
	Starter code for Assignment 3

	Implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"
#include <algorithm>

using namespace std;

// reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
// Phong Illumination Model thanks to reflection vector formula
void PointLight::shade(Ray3D& ray) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.
	double Ray_specular_exp = ray.intersection.mat->specular_exp;
	Color Ray_ambient = ray.intersection.mat->ambient;
	Color Ray_diffuse = ray.intersection.mat->diffuse;
	Color Ray_specular = ray.intersection.mat->specular;

	//view vetor in world 
	Vector3D view = -ray.dir;
	view.normalize();
	//light vector in world
	Vector3D light = pos - ray.intersection.point;
	light.normalize();
	//normal vector in world
	Vector3D normal = (ray.intersection.normal);
	normal.normalize();
	//reflect vector in world
	Vector3D reflect = (2.0 * light.dot(normal)*normal) - light;
	reflect.normalize();

	Color total_ambient = Ray_ambient * col_ambient;
	//really weird here, why max(0,0,...) is different as max(...,0.0).
	Color total_diffuse = Ray_diffuse * (max(0.0, light.dot(normal)) * col_diffuse);
	Color total_specular = Ray_specular * (max(0.0, pow(reflect.dot(view),Ray_specular_exp)) * col_specular);

	ray.col = total_ambient + total_diffuse + total_specular;

	ray.col.clamp();  
}



