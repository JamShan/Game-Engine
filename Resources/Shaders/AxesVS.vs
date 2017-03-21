attribute vec3 a_posL;
attribute vec3 a_color;


varying vec3 v_color;


uniform mat4 u_MVP;


void main()
{
	vec4 posL = vec4(a_posL, 1.0);
	posL = u_MVP*posL;
	// outputs 
	v_color = a_color;
	gl_Position = posL;
	
}