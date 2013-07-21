uniform sampler2D SrcAddA;
uniform sampler2D SrcAddB;

flat in float vAlpha;
in vec2 vCoord;

out float FragColor0;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float add = texelFetch(SrcAddB, coord, 0).r + vAlpha*texelFetch(SrcAddA, coord, 0).r;
    
    FragColor0 = add;
}