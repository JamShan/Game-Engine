precision mediump float;
// varyings
varying vec2 v_uv;
//uniforms
uniform sampler2D u_texture;
float limit= 0.0;
void main()
{
    vec3 color = vec3(texture2D(u_texture,v_uv));
    float brightness = 0.3 * color.r + 0.59 * color.g + 0.11 * color.b;
    float valr = step(limit,brightness);
    gl_FragColor = valr*vec4(color,1.0);
  
    
}
