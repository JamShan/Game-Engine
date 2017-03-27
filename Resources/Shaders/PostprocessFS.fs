precision mediump float;
// varyings
varying vec2 v_uv;
//uniforms
uniform sampler2D u_texture;

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
        sampleTex[i] = vec3(texture2D(u_texture, v_uv.st + offsets[i]));
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
    kernel[0] = boxBlur[0];
    kernel[1] = boxBlur[1];
    kernel[2] = boxBlur[2];
    kernel[3] = boxBlur[3];
    kernel[4] = boxBlur[4];
    kernel[5] = boxBlur[5];
    kernel[6] = boxBlur[6];
    kernel[7] = boxBlur[7];
    kernel[8] = boxBlur[8];
}
void main()
{
    prepareKernels();
    setKernel();
    // inverse color
    vec4 inverse = vec4(vec3(1.0-vec3(texture2D(u_texture,v_uv))),1.0);

    // greyscale
    vec4 cul =  texture2D(u_texture,v_uv);
    float greyscale = cul.r *0.3 + cul.g*0.5 + cul.b * 0.2;
    vec4 grey = vec4(greyscale,greyscale,greyscale,1.0);
    
    float step = 0.20/100.0;
    gl_FragColor = cul;//convolution(step);//inverse;
    
}
