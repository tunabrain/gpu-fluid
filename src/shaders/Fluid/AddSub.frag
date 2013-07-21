uniform sampler2D SrcSubA;
uniform sampler2D SrcSubB;
uniform sampler2D SrcAddA;
uniform sampler2D SrcAddB;

flat in float vAlpha;
in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float sub = texelFetch(SrcSubA, coord, 0).r - vAlpha*texelFetch(SrcSubB, coord, 0).r;
    float add = texelFetch(SrcAddA, coord, 0).r + vAlpha*texelFetch(SrcAddB, coord, 0).r;
    
    FragColor0 = sub;
    FragColor1 = add;
}