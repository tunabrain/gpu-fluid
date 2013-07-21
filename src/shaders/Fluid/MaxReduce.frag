uniform sampler2D R;
uniform ivec2 Inner;

in vec2 vCoord;

out float FragColor0;

void main() {
    ivec2 coord = ivec2(vCoord - 0.5);
    
    if (coord.x >= Inner.x || coord.y >= Inner.y) {
        FragColor0 = 0.0;
    } else {
        float A1 = texelFetch      (R, coord, 0             ).r;
        float A2 = texelFetchOffset(R, coord, 0, ivec2(1, 0)).r;
        float A3 = texelFetchOffset(R, coord, 0, ivec2(0, 1)).r;
        float A4 = texelFetchOffset(R, coord, 0, ivec2(1, 1)).r;
        
        FragColor0 = max(abs(A1), max(abs(A2), max(abs(A3), abs(A4))));
    }
}