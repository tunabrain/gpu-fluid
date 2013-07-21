uniform usampler2D Counts;
uniform usampler2D Offsets;
layout(pixel_center_integer) in vec4 gl_FragCoord;

layout(rg32f) uniform image2D ResultP;
layout(rgba32f) uniform image2D ResultQ;

uniform ivec2 PointInfo;

vec2 rand(uvec2 p) {
    const uint M = 1664525u, C = 1013904223u;
    uint seed = (p.x*M + p.y + C)*M;
    seed ^= (seed >> 11u);
    seed ^= (seed << 7u) & 0x9d2c5680u;
    seed ^= (seed << 15u) & 0xefc60000u;
    seed ^= (seed >> 18u);
    float x = uintBitsToFloat(seed >> 8u | 0x3F800000u) - 1.0;
    seed = (1103515245u*seed + 12345u) & 0x7FFFFFFFu;
    return vec2(x, uintBitsToFloat(seed >> 8u | 0x3F800000u) - 1.0);
}

void main() {
    const float marker = uintBitsToFloat(0xDEADBEEFu);
    
    ivec2 iCoord = ivec2(gl_FragCoord.xy);
    
    int offset = int(texelFetch(Offsets, iCoord, 0).r);
    int count  = int(texelFetch(Counts,  iCoord, 0).r & 0xFFFFFFF) - 0x8000000;
    
    for (int i = 0; i < 3; i++) {
        if (i < count) {
            ivec2 coord = ivec2((offset + i) % PointInfo.x, (offset + i)/PointInfo.x);
            
            imageStore(ResultP, coord, (vec2(iCoord) + rand(uvec2(coord))).xyxy);
            imageStore(ResultQ, coord, vec4(marker));
        }
    }
}