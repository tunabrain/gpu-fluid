uniform sampler2D A;
uniform sampler2D B;

in vec2 vCoord;

out float FragColor0;

void main() {
    const float marker = uintBitsToFloat(0xDEADBEEFu);
    
    ivec2 coord = ivec2(vCoord);
    
    float D = texelFetch(B, coord, 0).r;
    
    if (D == 0.0)
        FragColor0 = marker;
    else
        FragColor0 = texelFetch(A, coord, 0).r/D;
}