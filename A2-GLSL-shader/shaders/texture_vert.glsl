attribute vec3 position; // Given vertex position in object space
attribute vec3 normal;  // Given vertex normal in object space
attribute vec2 texCoord; // Given texture (uv) coordinates
attribute vec3 worldPosition; // Given vertex position in world space

uniform mat4 projection, modelview, normalMat; // Given scene transformation matrices
uniform vec3 eyePos;	// Given position of the camera/eye/viewer

// These will be given to the fragment shader and interpolated automatically
varying vec3 normalInterp; // Normal
varying vec3 vertPos; // Vertex position
varying vec3 viewVec; // Vector from the eye to the vertex
varying highp vec2 texCoordInterp;

void main() {
  // Your solution should go here.
  vec4 vertPos4 = modelview * vec4(position, 1.0);
  //texCoordInterp.x = 0.5 + atan(vertPos4.x,vertPos4.z) /(2.0 * 3.1415926535);
  //texCoordInterp.y = 0.5 - asin(vertPos4.y) /(3.1415926535);
  //vec4 texCoord = vec4(position,1);(varying)
  //well... the texCoord has been given in 2D. Therefore, don't need to convert object vertex in 4D to 2D...
  texCoordInterp = texCoord;
  gl_Position = projection * vertPos4;
}
