uniform sampler2D PPos;
uniform sampler2D Q;
uniform ivec2 PointInfo;
uniform vec2 Scale;
uniform vec2 Offset;

flat out float vQ;

void main() {
    ivec2 coord = ivec2(gl_VertexID % PointInfo.x, gl_VertexID/PointInfo.x);
    vec2 pos = texelFetch(PPos, coord, 0).xy;
    
    vQ = texelFetch(Q, coord, 0).r;
    
    gl_Position = vec4((pos - Offset)*Scale*2.0 - 1.0, 0.0, 1.0);
}