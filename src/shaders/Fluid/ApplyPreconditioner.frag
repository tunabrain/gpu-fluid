uniform sampler2D R;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float C = texelFetch(R, coord, 0).r;
    
    float A  = 9.0/8.0*C;
    float A2 = 1.0/4.0*texelFetchOffset(R, coord, 0, ivec2( 1,  0)).r;
    float A4 = 1.0/4.0*texelFetchOffset(R, coord, 0, ivec2( 0,  1)).r;
    float A1 = 0.0, A3 = 0.0;
    if (coord.x > 0)
        A1 = 1.0/4.0*texelFetchOffset(R, coord, 0, ivec2(-1,  0)).r;
    if (coord.y > 0)
        A3 = 1.0/4.0*texelFetchOffset(R, coord, 0, ivec2( 0, -1)).r;
    
    A += A1 + A2 + A3 + A4;
    
    FragColor0 = A;
    FragColor1 = A*C;
}