layout(rg32f) uniform image2D ResultP;
layout(rgba32f) uniform image2D ResultQ;

uniform ivec2 PointInfo;
uniform vec4 QuadInfo;
uniform vec4 QMin;
uniform vec4 QValue;

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
    ivec2 coord = ivec2(gl_VertexID % PointInfo.x, gl_VertexID/PointInfo.x);
    vec2 W = rand(uvec2(coord));
    
    vec2 pos = QuadInfo.xy + QuadInfo.zw*W;
    W = 1.0 - abs(W*2.0 - 1.0);
    vec4 qs  = QMin + QValue*W.x;
    
    imageStore(ResultP, coord, pos.xyxy);
    imageStore(ResultQ, coord, qs);
    
    gl_Position = vec4(10000.0, 10000.0, 10000.0, 1.0);
}