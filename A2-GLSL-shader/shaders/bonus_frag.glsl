// Fragment shader template for the bonus question

precision mediump float; // It is required to set a floating point precision in all fragment shaders.

// Interpolated values from vertex shader
// NOTE: You may need to edit this section to add additional variables
varying vec3 normalInterp; // Normal
varying vec3 vertPos; // Vertex position
varying vec3 viewVec; // Interpolated view vector

// uniform values remain the same across the scene
// NOTE: You may need to edit this section to add additional variables
uniform float Ka;   // Ambient reflection coefficient
uniform float Kd;   // Diffuse reflection coefficient
uniform float Ks;   // Specular reflection coefficient
uniform float shininessVal; // Shininess

// Material color
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform vec3 lightPos; // Light position in camera space

uniform sampler2D uSampler;	// 2D sampler for the earth texture

//wikipedia formula
float beckmann_distribution(in float alpha, in float m){
	return (exp(-tan(alpha)*tan(alpha)/(m*m)))/(3.1415926535*m*m*cos(alpha)*cos(alpha)*cos(alpha)*cos(alpha));
}

//wikipedia formula
float geometric_attenuation(in float h_dot_n, in float v_dot_n, in float v_dot_h, in float l_dot_n){
	return min(min(2.0*(h_dot_n)*(v_dot_n)/(v_dot_h),2.0*(h_dot_n)*(l_dot_n)/(v_dot_h)),1.0);
}

//wikipedia formula
float calculate_cook_torrance(in float v_dot_n, in float n_dot_l, in float beckmann, in float fresnel, in float geometric_attenuation){
	return (beckmann*fresnel*geometric_attenuation)/(4.0*v_dot_n*n_dot_l);
}


void main() {
  // Your solution should go here.
  // Only the ambient colour calculations have been provided as an example.

  //implement cook-torrance model by wikipedia math formula

  //light vector L
  vec3 lightVec = normalize(lightPos - vertPos); //(L)
  //half vector H
  vec3 halfVector = normalize(viewVec + lightVec);
  //view vector V  (viewVec)
  //normal vector N (normalInterp)

  float v_dot_n = max(dot(viewVec,normalInterp),0.0);
  float n_dot_l = max(dot(normalInterp,lightVec),0.0);
  float h_dot_n = max(dot(halfVector,normalInterp),0.0);
  float v_dot_h = max(dot(viewVec,halfVector),0.0);
  float l_dot_n = max(dot(lightVec,normalInterp),0.0);

  float n_dot_h = max(dot(normalInterp,halfVector),0.0);

  float alpha = acos(n_dot_h);
  //give a value for m
  float m = 1.0;

  //give a value for fresnel
  float fresnel = 1.0;

  float Kspec = calculate_cook_torrance(v_dot_n,n_dot_l,beckmann_distribution(alpha,m),fresnel,geometric_attenuation(h_dot_n,v_dot_n,v_dot_h,l_dot_n));

  //r*v
  float r_dot_v = max(dot(reflect(-lightVec,normalInterp),viewVec),0.0);

  vec3 final_color = Ka * ambientColor + Kd * diffuseColor * n_dot_l + Kspec * Ks * specularColor * pow(r_dot_v,shininessVal);

  gl_FragColor = vec4(final_color, 1.0);
}
