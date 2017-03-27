
// attributes
attribute vec3 a_posL;
attribute vec2 a_uv;
// varyings 
varying vec2 v_uv;
// locals
void main()
{
	// outputs 
	v_uv = a_uv;
	gl_Position = vec4(a_posL,1.0);

}