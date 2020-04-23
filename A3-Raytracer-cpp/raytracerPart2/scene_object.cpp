/***********************************************************
	
	Starter code for Assignment 3
	
	Implements scene_object.h

***********************************************************/

#include <cmath>
#include "scene_object.h"

// reference on tutorial slide Ray Tracing  - Part 1 from TA Muhammed Anwar & Kevin Gibson
// Ray – Unit Plane Intersection Pseudocode
bool UnitSquare::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Point3D originObj = worldToModel * ray.origin;
	Vector3D objDir = worldToModel * ray.dir;
	Ray3D objRay(originObj,objDir);

	double t = -(originObj[2]/objDir[2]);

	//t <= 0 then return false
	if(t <= 0 || objDir[2] == 0){
		return false;
	}

	double x_value,y_value;
	Vector3D planeNorm(0,0,1);

	x_value = originObj[0] + t*objDir[0];
	y_value = originObj[1] + t*objDir[1];

	//determine if x,y in valid region
	if(x_value <= 0.5 && x_value >= -0.5 && y_value <= 0.5 && y_value >= -0.5 ){
		//determine if needed to update it as a minimum t_value
		if(ray.intersection.none || t < ray.intersection.t_value ){
			ray.intersection.t_value = t;
			ray.intersection.none = false;
			ray.intersection.point = modelToWorld * Point3D(x_value,y_value,0);
			ray.intersection.normal = transNorm(worldToModel,planeNorm);
			return true;
		}
	}


	return false;
}

//reference on http://www.dgp.toronto.edu/~karan/courses/418/slides/lecture8a.compressed.pdf
//Intersections between rays and spheres
bool UnitSphere::intersect(Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	Point3D originObj = worldToModel * ray.origin;
	Vector3D objDir = worldToModel * ray.dir;
	Ray3D objRay(originObj,objDir);

	//get (e - c)
	Vector3D e_c = originObj - Point3D(0,0,0);

	double a = objDir.dot(objDir);
	double b = 2*objDir.dot(e_c);
	double c = e_c.dot(e_c)-1;

	//root determination b^2 - 4ac 
	double rootTest = b*b - 4*a*c;
	double root1,root2, minRoot;

	//quadratic formula
	double result1 = (-b + sqrt(b*b-4*a*c))/(2*a);
	double result2 = (-b - sqrt(b*b-4*a*c))/(2*a);

	//no intersections
	if(rootTest < 0){
		return false;
	}

	if(rootTest > 0){
		if(result1 < 0 && result2 < 0){
			return false;
		}else if(result1 < 0){
			minRoot = result2;
		}else if(result2 < 0){
			minRoot = result1;
		}else{
			minRoot = result1 > result2 ? result2 : result1;
		}
	}

	//determine if needed to update minRoot in multiple objects
	if(ray.intersection.none|| minRoot < ray.intersection.t_value){
	    Point3D intersectionPoint = originObj + minRoot*objDir ;  
	    Vector3D interVec = intersectionPoint - Point3D(0,0,0);
	    interVec.normalize();
	    //store and convert xyz model space to uv space
		convert_to_uvCoord(ray, interVec);
	    ray.intersection.point = modelToWorld*intersectionPoint;
		ray.intersection.normal = transNorm(worldToModel,interVec);
		ray.intersection.t_value = minRoot;
		ray.intersection.none = false;
		return true;
	}

	return false;
}

//https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
void convert_to_uvCoord(Ray3D &ray, Vector3D &interVec){
	//pointing to north pole
	Vector3D Vn(0,1,0);
	Vector3D Ve(0,0,1);
	Vector3D Vp = interVec;

	double phi = acos(-Vn.dot(Vp));

	double v = phi / M_PI;

	double theta = (acos(Vp.dot(Ve)/sin(phi)))/(2*M_PI);

	Vector3D VnCrossVe = Vn.cross(Ve);

	double u;

	if(VnCrossVe.dot(Vp) > 0){
		u = theta;
	}else{
		u = 1 - theta;
	}

	//save to struct
	ray.u_coord = u;
	ray.v_coord = v;
}

void SceneNode::rotate(char axis, double angle) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
			this->trans = this->trans*rotation; 	
			angle = -angle;
		} 
		else {
			this->invtrans = rotation*this->invtrans; 
		}	
	}
}

void SceneNode::translate(Vector3D trans) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	this->trans = this->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	this->invtrans = translation*this->invtrans; 
}

void SceneNode::scale(Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	this->trans = this->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	this->invtrans = scale*this->invtrans; 
}


