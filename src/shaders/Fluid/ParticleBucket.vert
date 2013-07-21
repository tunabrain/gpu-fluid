layout(r32ui) uniform uimage2D Counts;
layout(rg32f) uniform image2D ResultP;
layout(rgba32f) uniform image2D ResultQ;

uniform usampler2D Offsets;
uniform sampler2D PPos;
uniform sampler2D Q;

uniform ivec2 PointInfo;

void main() {
    ivec2 coord = ivec2(gl_VertexID % PointInfo.x, gl_VertexID/PointInfo.x);
    vec2 pos = texelFetch(PPos, coord, 0).xy;
    vec4 qs  = texelFetch(Q, coord, 0);
    
    ivec2 iPos = ivec2(pos - 0.0);
    
    int bucketOffset = int(imageAtomicAdd(Counts, iPos, -1) & 0xFFFFFFF) - 0x8000001;
    
    if (bucketOffset < 0)
        return;
    
    int offset = int(texelFetch(Offsets, iPos, 0).r) + bucketOffset;
    coord = ivec2(offset % PointInfo.x, offset/PointInfo.x);
    
    imageStore(ResultP, coord, pos.xyxy);
    imageStore(ResultQ, coord, qs);
    
    gl_Position = vec4(10000.0, 10000.0, 10000.0, 1.0);
}