
// attributes
attribute vec3 a_posL;
attribute vec2 a_uv;
attribute vec3 a_norm;
// varyings 
varying vec2 v_uv;
varying vec3 v_fog;
varying vec3 v_Wnorm;
// uniforms 
uniform mat4 u_MVP,
			 u_model;
// locals
void main()
{
	
	v_uv = a_uv;
	vec4 posL = vec4(a_posL, 1.0);
	vec4 vertexPos = u_model*posL;
	posL = u_MVP*posL;
	// outputs 
	v_fog = vec3(vertexPos);
	gl_Position = posL;
    vec4 normal = vec4(a_norm,0.0);
    v_Wnorm =normalize((u_model*normal).xyz); 
	
}
   