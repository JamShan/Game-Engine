precision mediump float;
// varyings
varying vec2 v_uv;
varying vec3 v_fog;
//uniforms
uniform sampler2D u_fire,
                  u_disp,
                  u_mask;

uniform float u_wired,
              u_r,
              u_R,
              // data for fire
              u_Time,
              u_DispMax;
uniform vec3 u_fog_color,
             u_cameraCoord;      

void main()
{
    
    vec2 disp = texture2D(u_disp, vec2(v_uv.x, v_uv.y + u_Time)).rg;
    vec2 offset = (disp*2.0 -vec2(1.0,1.0))*u_DispMax;
    vec2 v_uv_displaced = v_uv+offset;
    vec4 c_fire = texture2D(u_fire,v_uv_displaced);
    vec4 c_alpha = texture2D(u_mask,vec2(v_uv.x,-v_uv.y));
    c_fire.a *= c_alpha.r;
    /////////////////////////////////////////////////////////////////////
    // fog stuff
    // get the color
    vec4 c_final = c_fire;
    float distance  = length(u_cameraCoord -v_fog);
    float sol = (distance-u_r)/(u_R-u_r);
    float alpha = clamp(sol,0.0,1.0); 
    vec4 culoare= vec4(u_fog_color,1.0)*alpha + c_final*(1.0-alpha);
   
    // outputs
    gl_FragColor = culoare*(1.0-u_wired) +  vec4(0.5,0.5,0.5,1.0)*u_wired; 
     
    
}
