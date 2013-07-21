uniform sampler2D R;
uniform ivec2 Inner;

in vec2 vCoord;

out float FragColor0;

void main() {
    const vec2 scale = vec2(1.0/WIDTH, 1.0/HEIGHT);
    
    vec2 coord = vCoord*scale;
    
    if (int(vCoord.x) >= Inner.x || int(vCoord.y) >= Inner.y) {
        FragColor0 = 0.0;
    } else {
        float A1 = textureOffset(R, coord, ivec2(-1, -1)).r;
        float A2 = textureOffset(R, coord, ivec2( 1, -1)).r;
        float A3 = textureOffset(R, coord, ivec2( 1,  1)).r;
        float A4 = textureOffset(R, coord, ivec2(-1,  1)).r;
        
        FragColor0 = (A1 + A2 + A3 + A4)*4.0;
    }
}