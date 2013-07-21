uniform usampler2D Counts;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out uint FragColor0;

void main() {
    ivec2 Base = ivec2(gl_FragCoord.xy)*2;
    
    FragColor0 = 0x7FFFFFF &
         (texelFetch      (Counts, Base, 0             ).r
        + texelFetchOffset(Counts, Base, 0, ivec2(1, 0)).r
        + texelFetchOffset(Counts, Base, 0, ivec2(0, 1)).r
        + texelFetchOffset(Counts, Base, 0, ivec2(1, 1)).r);
}