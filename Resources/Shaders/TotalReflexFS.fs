precision mediump float;
// varyings

varying vec2 v_uv;
varying vec3 v_fog; // position
varying vec3 v_Wnorm; // normal
//uniforms
uniform samplerCube u_cube_texture;  

uniform float u_wired,
              u_r,
              u_R;
uniform vec3 u_fog_color,
             u_cameraCoord;      
////////////////////////////////////////////
// lights

// object data
uniform float u_ratio,
              u_diffuseCoeficient,
              u_specularCoeficient;
// ambiental light from entire scene
uniform vec3 u_ambientalColor;
// light data
struct Light
{
    vec4 Type;// 1,0,0,0 for directional 0,1,0,0 for point, 0,0,1,0 for spot light, 0,0,0,1
    vec3 position, // where the light is  located if spot or point
         direction;// direction of the light
    vec3 specularColor,
         diffuseColor;
    float shiness;
};
// we assume there an object has maximum 4 lights associated
uniform Light u_lights[4];
void main()
{

    // skybox reflexion
    vec3 vectCam = u_cameraCoord - v_fog;
    vec3 dirReflect = reflect( normalize(vectCam), normalize(v_Wnorm) );
    // fog stuff
    // get the texture
    vec3 objColor = vec3(textureCube(u_cube_texture, dirReflect));

     
    // lighting
    vec3 c_light_sum = vec3(0.0,0.0,0.0); // here we store the composed lights
    
    // light stuff

    // ambiental light is same for all types of light so we add it just once
    vec3 ambientalComponent = objColor * u_ambientalColor * u_ratio;
    
    // 
   
    {
        const int i = 0;
        // so we don't do if statement to see what type of light we use     
        // we instead calculate the 4 types of light and then we add them togheter 
        // using the Type parameter and perform something like a dot product 
        
        // we first calculate the directional light
        ///////////////////////////////////////////////////////////////////////////////////
        vec3 directionalType= vec3(0.0,0.0,0.0);
        // 
        {
            // diffuse light

            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = u_lights[i].direction; 
           
           
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light

            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            directionalType = diffuseComponent + specularComponent;
         }
      
        ////////////////////////////////////////////////////////////////////////////////////////////
        // point light
        vec3 pointType=vec3(0.0,0.0,0.0);
        {
            
            // diffuse light
            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = normalize(v_fog - u_lights[i].position); 
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light
            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            pointType = diffuseComponent + specularComponent;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        // spot light
        vec3 spotType=vec3(0.0,0.0,0.0);







        /////////////////////////////////////////////////////////////////////////////////////////////
        // area light
        vec3 areaType= vec3(0.0,0.0,0.0);






        /////////////////////////////////////////////////////////////////////////////////////////////
        // we see what type of light we have

        c_light_sum += directionalType * u_lights[i].Type.x +
                       pointType       * u_lights[i].Type.y +
                       spotType        * u_lights[i].Type.z +
                       areaType        * u_lights[i].Type.w ;

    }
    {
        const int i = 1;
        // so we don't do if statement to see what type of light we use     
        // we instead calculate the 4 types of light and then we add them togheter 
        // using the Type parameter and perform something like a dot product 
        
        // we first calculate the directional light
        ///////////////////////////////////////////////////////////////////////////////////
        vec3 directionalType= vec3(0.0,0.0,0.0);
        // 
        {
            // diffuse light

            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = u_lights[i].direction; 
           
           
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light

            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            directionalType = diffuseComponent + specularComponent;
         }
      
        ////////////////////////////////////////////////////////////////////////////////////////////
        // point light
        vec3 pointType=vec3(0.0,0.0,0.0);
        {
            
            // diffuse light
            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = normalize(v_fog - u_lights[i].position); 
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light
            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            pointType = diffuseComponent + specularComponent;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        // spot light
        vec3 spotType=vec3(0.0,0.0,0.0);







        /////////////////////////////////////////////////////////////////////////////////////////////
        // area light
        vec3 areaType= vec3(0.0,0.0,0.0);






        /////////////////////////////////////////////////////////////////////////////////////////////
        // we see what type of light we have

        c_light_sum += directionalType * u_lights[i].Type.x +
                       pointType       * u_lights[i].Type.y +
                       spotType        * u_lights[i].Type.z +
                       areaType        * u_lights[i].Type.w ;

    }
    {
        const int i = 2;
        // so we don't do if statement to see what type of light we use     
        // we instead calculate the 4 types of light and then we add them togheter 
        // using the Type parameter and perform something like a dot product 
        
        // we first calculate the directional light
        ///////////////////////////////////////////////////////////////////////////////////
        vec3 directionalType= vec3(0.0,0.0,0.0);
        // 
        {
            // diffuse light

            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = u_lights[i].direction; 
           
           
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light

            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            directionalType = diffuseComponent + specularComponent;
         }
      
        ////////////////////////////////////////////////////////////////////////////////////////////
        // point light
        vec3 pointType=vec3(0.0,0.0,0.0);
        {
            
            // diffuse light
            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = normalize(v_fog - u_lights[i].position); 
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light
            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            pointType = diffuseComponent + specularComponent;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        // spot light
        vec3 spotType=vec3(0.0,0.0,0.0);







        /////////////////////////////////////////////////////////////////////////////////////////////
        // area light
        vec3 areaType= vec3(0.0,0.0,0.0);






        /////////////////////////////////////////////////////////////////////////////////////////////
        // we see what type of light we have

        c_light_sum += directionalType * u_lights[i].Type.x +
                       pointType       * u_lights[i].Type.y +
                       spotType        * u_lights[i].Type.z +
                       areaType        * u_lights[i].Type.w ;

    }
    {
        const int i = 3;
        // so we don't do if statement to see what type of light we use     
        // we instead calculate the 4 types of light and then we add them togheter 
        // using the Type parameter and perform something like a dot product 
        
        // we first calculate the directional light
        ///////////////////////////////////////////////////////////////////////////////////
        vec3 directionalType= vec3(0.0,0.0,0.0);
        // 
        {
            // diffuse light

            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = u_lights[i].direction; 
           
           
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light

            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            directionalType = diffuseComponent + specularComponent;
         }
      
        ////////////////////////////////////////////////////////////////////////////////////////////
        // point light
        vec3 pointType=vec3(0.0,0.0,0.0);
        {
            
            // diffuse light
            // normal 
            vec3 N = normalize(v_Wnorm);
            // light direction
            vec3 L = normalize(v_fog - u_lights[i].position); 
            vec3 diffuseComponent = u_diffuseCoeficient * objColor * u_lights[i].diffuseColor * max(dot(N,-L), 0.0) *(1.0-u_ratio);
    
            // specular light
            // reflected light
            vec3 R = reflect(L,N);
            // vector from fragment to camera
            vec3 E = normalize(u_cameraCoord-v_fog);
            vec3 specularComponent = u_specularCoeficient * u_lights[i].specularColor * pow( max( dot( R, E ), 0.0 ), u_lights[i].shiness ) * ( 1.0- u_ratio);
              // we add the diffuse and specular components
            pointType = diffuseComponent + specularComponent;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////
        // spot light
        vec3 spotType=vec3(0.0,0.0,0.0);







        /////////////////////////////////////////////////////////////////////////////////////////////
        // area light
        vec3 areaType= vec3(0.0,0.0,0.0);






        /////////////////////////////////////////////////////////////////////////////////////////////
        // we see what type of light we have

        c_light_sum += directionalType * u_lights[i].Type.x +
                       pointType       * u_lights[i].Type.y +
                       spotType        * u_lights[i].Type.z +
                       areaType        * u_lights[i].Type.w ;

    }
    
    c_light_sum += ambientalComponent;
    /////////////////////////////////////////////////////////////////////////////////////////

     // fog stuff
    float distance  = length(u_cameraCoord -v_fog);
    float sol = (distance-u_r)/(u_R-u_r);
    float alpha = clamp(sol,0.0,1.0); 
    vec4 finalColor= vec4(u_fog_color,1.0)*alpha + vec4(c_light_sum,1.0)*(1.0-alpha);
    // color after fog
    ////////////////////////////////////////////////////////////////////////////////////
    // outputs wired or not wired
    gl_FragColor = finalColor*(1.0-u_wired) +  vec4(0.5,0.5,0.5,1.0)*u_wired; 

}
