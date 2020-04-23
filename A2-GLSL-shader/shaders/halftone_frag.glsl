precision mediump float; // It is required to set a floating point precision in all fragment shaders.

// Interpolated values from vertex shader
varying vec3 normalInterp; // Normal
varying vec3 vertPos; // Vertex position
varying vec3 viewVec; // Interpolated view vector

// uniform values remain the same across the scene
uniform float Ka;   // Ambient reflection coefficient
uniform float Kd;   // Diffuse reflection coefficient
uniform float Ks;   // Specular reflection coefficient
uniform float shininessVal; // Shininess

// Material color
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform vec3 lightPos; // Light position in camera space

// HINT: Use the built-in variable gl_FragCoord to get the screen-space coordinates

void main() {
//*****************************Basic Phong shading calculation for color **********************************************************************************
  // Your solution should go here.
  // Only the background color calculations have been provided as an example.

  vec3 lightVec = lightPos - vertPos;
  lightVec = normalize(lightVec);
  //normalInterp = normalize(normalInterp);
  float diffuse = Kd * dot(lightVec,normalInterp);	
  //why max function is not supported here....
  if(diffuse < 0.0)
    diffuse = 0.0;
  //viewVec = normalize(viewVec);

  //method 1: reflection with view algorithm
  //result: closer to handout picture.(personally like this display)
  vec3 r = reflect(-lightVec,normalInterp);
  float r_dot_v = dot(normalize(r), viewVec);
  //why max function is not supported here....
  if(r_dot_v < 0.0)
  	r_dot_v = 0.0;
  float specular = Ks * pow(r_dot_v, shininessVal);

  vec3 total_color = Ka  * ambientColor + diffuse * diffuseColor + specular * specularColor;

  gl_FragColor = vec4(total_color, 1.0);
 //*******************************************************************************************************************************************************
 //Reference: http://weber.itn.liu.se/~stegu/webglshadertutorial/shadertutorial.html
}