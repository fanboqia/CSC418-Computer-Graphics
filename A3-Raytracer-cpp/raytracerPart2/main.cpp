/***********************************************************
	
	Starter code for Assignment 3

***********************************************************/

#include <cstdlib>
#include "raytracer.h"

#define glossy_on true

int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;
	LightList light_list;
	Scene scene;   

	int width = 320;
	int height = 240;
	int depth = 5;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	//add texture file
	char fileName[] = "sun.bmp";
//	unsigned long int widthBMP;
//	long int heightBMP;
//	unsigned char *rarray;
//	unsigned char *garray;
//	unsigned char *barray;

	Texture sphereTexture(fileName);
//	if(bmp_read((char* )fileName,&widthBMP,&heightBMP,&rarray,&garray,&barray)){
//		std::cout<<"texture file load success!!!"<<std::endl;
//	}else{
//		std::cout<<"bmp_read_texture_file_fail..."<<std::endl;
//		return -1;
//	}

	
	// // Define materials for shading.
	 Material gold(Color(0.3, 0.3, 0.3), Color(0.75164,0.60648,0.22648),
	 	Color(0.628281, 0.555802, 0.366065),
	 	51.2, &sphereTexture, glossy_on);
	 Material jade(Color(0, 0, 0), Color(0.54,0.89,0.63),
	 	Color(0.316228,0.316228,0.316228),
	 	12.8, NULL, !glossy_on);
	 Material mirror(Color(0.99,0.99,0.99),Color(1.0, 1.0, 1.0),
	 	Color(0.99,0.99,0.99),
	 	1000, NULL, !glossy_on);
	 Material blue(Color(51.0/255,119.0/255,1.0),Color(51.0/255,119.0/255,1.0),
	 	Color(0.99,0.99,0.99),
	 	1000, NULL, !glossy_on);
	 Material glass(Color(0.0,0.0,0.0),Color(0.5, 0.5, 0.5),
	 	Color(0.5,0.5,0.5),
	 	100, NULL, !glossy_on);

	// // Defines a point light source.
	 PointLight* pLight = new PointLight(Point3D(0,0,5), Color(0.9,0.9,0.9));
	 light_list.push_back(pLight);

	 //add more lights 
	 // for(unsigned i = 0; i < 20; i++){
	 // 	light_list.push_back(new PointLight(Point3D(i,i,i), Color(1.0,1.0,1.0)));
	 // }
	
	// // Add a unit square into the scene with material mat.
	 SceneNode* sphere = new SceneNode(new UnitSphere(), &gold);
	 scene.push_back(sphere);
	 SceneNode* plane = new SceneNode(new UnitSquare(), &jade);
	 scene.push_back(plane);

	// //***********new added sphere*****************
	 SceneNode* sphere2 = new SceneNode(new UnitSphere(), &glass);
	 scene.push_back(sphere2);

	 // Apply some transformations to the sphere and unit square.
	 double factor1[3] = { 1.0, 1.0, 1.0 };
	 sphere->translate(Vector3D(0, 0, -5));
	 sphere->rotate('x', -45);
	 sphere->rotate('z', 45);
	 sphere->scale(Point3D(0, 0, 0), factor1);

	 double factor2[3] = { 9.0, 9.0, 9.0 };
	 plane->translate(Vector3D(0, 0, -7));
	 plane->rotate('z', 45);
	 plane->scale(Point3D(0, 0, 0), factor2);

	 double factor3[3] = { 1.0, 1.0, 1.0 };
	 sphere2->translate(Vector3D(-3, 1, -5));
	 sphere2->rotate('x', -45);
	 sphere2->rotate('z', 45);
	 sphere2->scale(Point3D(0, 0, 0), factor3);

	 // Render the scene, feel free to make the image smaller for
	 // testing purposes.	
	 Camera camera1(Point3D(0, 0, 1), Vector3D(0, 0, -1), Vector3D(0, 1, 0), 60.0);
	 Image image1(width, height);
	 raytracer.render(camera1, scene, light_list, image1, depth); //render 3D scene to image
	 image1.flushPixelBuffer("view1.bmp"); //save rendered image to file

	 // Render it from a different point of view.
	 Camera camera2(Point3D(4, 2, 1), Vector3D(-4, -2, -6), Vector3D(0, 1, 0), 60.0);
	 Image image2(width, height);
	 raytracer.render(camera2, scene, light_list, image2, depth);
	 image2.flushPixelBuffer("view2.bmp");

	 // Free memory
	 for (size_t i = 0; i < scene.size(); ++i) {
	 	delete scene[i];
	 }

	 for (size_t i = 0; i < light_list.size(); ++i) {
	 	delete light_list[i];
	 }

	return 0;
}