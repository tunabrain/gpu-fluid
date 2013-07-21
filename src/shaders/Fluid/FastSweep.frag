uniform sampler2D D;
uniform sampler2D W;

layout(pixel_center_integer) in vec4 gl_FragCoord;

layout(r32f) uniform image2D Dst;

void main() {
    int y = int(gl_FragCoord.y);
    
    float Rep = 0.0;
    for (int x = 0; x < WIDTH; x++) {
        float D = texelFetch(D, ivec2(x, y), 0).r;
        float W = texelFetch(W, ivec2(x, y), 0).r;
        
        if (W != 0.0)
            Rep = D/W;
        
        imageStore(Dst, ivec2(x, y), vec4(Rep));
    }
}