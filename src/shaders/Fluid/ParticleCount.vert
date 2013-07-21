layout(r32ui) uniform uimage2D Counts;
uniform sampler2D PPos;

uniform ivec2 PointInfo;

void main() {
    ivec2 coord = ivec2(gl_VertexID % PointInfo.x, gl_VertexID/PointInfo.x);
    ivec2 iPos = ivec2(texelFetch(PPos, coord, 0).xy);
    imageAtomicAdd(Counts, iPos, 1);
    
    gl_Position = vec4(10000.0, 10000.0, 10000.0, 1.0);
}