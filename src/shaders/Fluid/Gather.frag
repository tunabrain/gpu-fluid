uniform sampler2D D;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out float FragColor0;

void main() {
    const float marker = uintBitsToFloat(0xDEADBEEFu);
    
    ivec2 coord = ivec2(gl_FragCoord.xy);
    
    float A = texelFetch(D, coord, 0).r;
    
    if (A == marker) {
        float A1 = marker, A2 = marker, A3 = marker, A4 = marker;
        if (coord.x > 0) A1 = texelFetchOffset(D, coord, 0, ivec2(-1,  0)).r;
        if (coord.y > 0) A2 = texelFetchOffset(D, coord, 0, ivec2( 0, -1)).r;
        if (coord.x < WIDTH - 1)  A3 = texelFetchOffset(D, coord, 0, ivec2(1, 0)).r;
        if (coord.y < HEIGHT - 1) A4 = texelFetchOffset(D, coord, 0, ivec2(0, 1)).r;
        
        vec2 Sum = vec2(0.0);
        
        Sum += (A1 == marker ? vec2(0.0) : vec2(A1, 1.0));
        Sum += (A2 == marker ? vec2(0.0) : vec2(A2, 1.0));
        Sum += (A3 == marker ? vec2(0.0) : vec2(A3, 1.0));
        Sum += (A4 == marker ? vec2(0.0) : vec2(A4, 1.0));
        
        A = (Sum.y == 0.0 ? marker : Sum.x/Sum.y);
    }
    
    FragColor0 = A;
}