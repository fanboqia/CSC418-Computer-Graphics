attribute vec3 position; // Given vertex position in object space
attribute vec3 normal; // Given vertex normal in object space
attribute vec3 worldPosition; // Given vertex position in world space

uniform mat4 projection, modelview, normalMat; // Given scene transformation matrices
uniform vec3 eyePos;	// Given position of the camera/eye/viewer

// These will be given to the fragment shader and interpolated automatically
varying vec3 normalInterp; // Normal
varying vec3 vertPos; // Vertex position
varying vec3 viewVec; // Vector from the eye to the vertex

void main()
{
  // Your solution should go here.
  vec4 vertPos4 = modelview * vec4(position, 1.0);

  viewVec = normalize(vec3(vec4(eyePos,1.0) - vertPos4));
  normalInterp = normalize(vec3(normalMat * vec4(normal,1.0)));
  vertPos = vec3(vertPos4);

  gl_Position = projection * vertPos4;
}