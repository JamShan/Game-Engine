precision mediump float;
// varyings
varying vec2 v_uv;
//uniforms
uniform sampler2D u_texture,//normal
                  u_texture2;// greyed
float kernel[9]; 
vec4 convolution(const float offset)
{
    highp vec2 offsets[9];

    offsets[0]= vec2(-offset, offset); // top-left
    offsets[1]=vec2(0.0,    offset);  // top-center
    offsets[2]=vec2(offset,  offset);  // top-right
    offsets[3]=vec2(-offset, 0.0);    // center-left
    offsets[4]=vec2(0.0,    0.0);   // center-center
    offsets[5]=vec2(offset,  0.0);    // center-right
    offsets[6]=vec2(-offset, -offset); // bottom-left
    offsets[7]=vec2(0.0,    -offset); // bottom-center
    offsets[8]=vec2(offset,  -offset);  // bottom-right    

    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture2D(u_texture2, v_uv.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
}
float boxBlur[9],
      gaussianBlur[9],
      sharpen[9],
      edgeDetection[9];
void prepareKernels()
{
    //box blur kernel
    boxBlur[0]= 1.0/9.0; boxBlur[1]= 1.0/9.0;   boxBlur[2]= 1.0/9.0;
    boxBlur[3]= 1.0/9.0; boxBlur[4]= 1.0/9.0;   boxBlur[5]= 1.0/9.0;
    boxBlur[6]= 1.0/9.0; boxBlur[7]= 1.0/9.0;   boxBlur[8]= 1.0/9.0;
    //gaussian blur kernel
    gaussianBlur[0]= 1.0/16.0; gaussianBlur[1]= 2.0/16.0;   gaussianBlur[2]= 1.0/16.0;
    gaussianBlur[3]= 2.0/16.0; gaussianBlur[4]= 4.0/16.0;   gaussianBlur[5]= 2.0/16.0;
    gaussianBlur[6]= 1.0/16.0; gaussianBlur[7]= 2.0/16.0;   gaussianBlur[8]= 1.0/16.0;
    // edge detection
    edgeDetection[0]= -1.0; edgeDetection[1]= -1.0;   edgeDetection[2]= -1.0;
    edgeDetection[3]= -1.0; edgeDetection[4]= 8.0;   edgeDetection[5]= -1.0;
    edgeDetection[6]= -1.0; edgeDetection[7]= -1.0;   edgeDetection[8]= -1.0;
    // sharpen
    sharpen[0]= 0.0; sharpen[1]= -1.0;   sharpen[2]= 0.0;
    sharpen[3]=-1.0; sharpen[4]= 5.0;      sharpen[5]= -1.0;
    sharpen[6]= 0.0; sharpen[7]= -1.0;   sharpen[8]= 0.0;

}
void setKernel()
{
    kernel[0] = gaussianBlur[0];
    kernel[1] = gaussianBlur[1];
    kernel[2] = gaussianBlur[2];
    kernel[3] = gaussianBlur[3];
    kernel[4] = gaussianBlur[4];
    kernel[5] = gaussianBlur[5];
    kernel[6] = gaussianBlur[6];
    kernel[7] = gaussianBlur[7];
    kernel[8] = gaussianBlur[8];
}
 
void main()
{
    prepareKernels();
    setKernel();
    vec4 norm = texture2D(u_texture,v_uv);
    // we get the bright colors blurred
    float step = 0.20/100.0;
    vec4 grey =  convolution(step);
    // and the result is the addition
    vec3 rez = vec3(norm+grey);
    gl_FragColor = norm;//vec4(rez,1.0);
    
}
