// attributes
attribute vec3 a_posL;
attribute vec2 a_uvsmall;
attribute vec2 a_uvlarge;
attribute vec3 a_norm;
// varyings
varying vec2 v_uv_large;
varying vec2 v_uv_small;
varying vec3 v_fog;
varying vec3 v_Wnorm;
// uniforms 
uniform sampler2D u_tex_blend;
uniform mat4 u_MVP,
			 u_model;
uniform vec3 u_height;
// locals
vec4 c_blend;

void main()
{
	vec4 vertexPos;
	
	c_blend = texture2D(u_tex_blend,a_uvlarge);
	vec4 posL = vec4(a_posL, 1.0);
	posL.y += c_blend.r*u_height.r+c_blend.g*u_height.g+c_blend.b*u_height.b;
	vertexPos = u_model*posL; 
	posL = u_MVP*posL;
	
	// outputs 
	gl_Position = posL;
	// texture
	v_uv_large = a_uvlarge;
	v_uv_small = a_uvsmall;
	// normal
	v_Wnorm = vec3(u_model* vec4(a_norm,0.0)); 
	// fog
	v_fog = vec3(vertexPos);
}
   