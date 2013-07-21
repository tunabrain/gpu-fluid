uniform usampler2D Counts;
uniform usampler2D Offsets;
layout(pixel_center_integer) in vec4 gl_FragCoord;

out uint FragColor0;

void main() {
    ivec2 IntCoord = ivec2(gl_FragCoord.xy);
    ivec2 OffsetBase = IntCoord/2;
    ivec2 CountBase = OffsetBase*2;
    
    int Idx = int(IntCoord.x > CountBase.x) + 2*int(IntCoord.y > CountBase.y);
    
    uint Offset = texelFetch(Offsets, OffsetBase, 0).r;
    if (Idx > 0) Offset += 0x7FFFFFF & texelFetch(Counts, CountBase, 0).r;
    if (Idx > 1) Offset += 0x7FFFFFF & texelFetchOffset(Counts, CountBase, 0, ivec2(1, 0)).r;
    if (Idx > 2) Offset += 0x7FFFFFF & texelFetchOffset(Counts, CountBase, 0, ivec2(0, 1)).r;
    
    FragColor0 = Offset;
}