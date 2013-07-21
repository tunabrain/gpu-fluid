uniform sampler2D ADiag;
uniform sampler2D APlusX;
uniform sampler2D APlusY;
uniform sampler2D B;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float C = texelFetch(B, coord, 0).r;
    
    float A  = texelFetch      (ADiag,  coord, 0               ).r*C;
    float A2 = texelFetch      (APlusX, coord, 0               ).r*texelFetchOffset(B, coord, 0, ivec2( 1,  0)).r;
    float A4 = texelFetch      (APlusY, coord, 0               ).r*texelFetchOffset(B, coord, 0, ivec2( 0,  1)).r;
    float A1 = 0.0, A3 = 0.0;
    if (coord.x > 0)
        A1 = texelFetchOffset(APlusX, coord, 0, ivec2(-1,  0)).r*texelFetchOffset(B, coord, 0, ivec2(-1,  0)).r;
    if (coord.y > 0)
        A3 = texelFetchOffset(APlusY, coord, 0, ivec2( 0, -1)).r*texelFetchOffset(B, coord, 0, ivec2( 0, -1)).r;
    
    A += A1 + A2 + A3 + A4;
    
    FragColor0 = A;
    FragColor1 = A*C;
}