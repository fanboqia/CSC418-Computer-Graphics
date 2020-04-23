attribute vec3 position; // Given vertex position in object space
attribute vec3 normal; // Given vertex normal in object space
attribute vec3 worldPosition; // Given vertex position in world space

uniform mat4 projection, modelview, normalMat; // Given scene transformation matrices
uniform vec3 eyePos;	// Given position of the camera/eye/viewer

// These will be given to the fragment shader and interpolated automatically
varying vec3 normalInterp; // Normal
varying vec3 vertPos; // Vertex position
varying vec3 viewVec; // Vector from the eye to the vertex
varying vec4 color;

uniform float Ka;   // Ambient reflection coefficient
uniform float Kd;   // Diffuse reflection coefficient
uniform float Ks;   // Specular reflection coefficient
uniform float shininessVal; // Shininess

// Material color
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 lightPos; // Light position in camera space


void main(){
  // Your solution should go here.
  // Only the ambient colour calculations have been provided as an example.
  vec3 vertNorPos3 = vec3(normalMat * vec4(normal,1.0)); 	
  vec3 vertPos3 = vec3(modelview * vec4(position, 1.0));
  vec3 lightVec = lightPos - vertPos3;
  lightVec = normalize(lightVec);
  vertNorPos3 = normalize(vertNorPos3);
  float diffuse = Kd * dot(lightVec,vertNorPos3);	
  //why max function is not supported here....
  if(diffuse < 0.0)
    diffuse = 0.0;
  vec3 eyeVec = normalize(eyePos - vertPos3);

  //method 1: halfVector h with normal vector n algorithm
  vec3 halfVec = eyeVec + lightVec;
  halfVec = normalize(halfVec);
  float n_dot_h = dot(halfVec, vertNorPos3);
  //why max function is not supported here....
  if(n_dot_h < 0.0)
    n_dot_h = 0.0;
  float specular = Ks * pow(n_dot_h, shininessVal);

  //method 2: reflection vector r with view vector  v algorithm
  //vec3 r = reflect(-lightVec,vertNorPos3);
  //r = normalize(r);
  //float r_dot_v = dot(r, eyeVec);
  //why max function is not supported here....
  //if(r_dot_v < 0.0)
  //  r_dot_v = 0.0;
  //float specular = Ks * pow(r_dot_v, shininessVal);

  vec3 final_color = Ka  * ambientColor + diffuse * diffuseColor + specular * specularColor;
  gl_Position = projection * vec4(vertPos3,1);
  color = vec4(final_color, 1.0); 
}

