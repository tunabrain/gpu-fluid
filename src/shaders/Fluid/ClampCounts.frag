uniform usampler2D Counts;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out uint FragColor0;

void main() {
    uint res = clamp(texelFetch(Counts, ivec2(gl_FragCoord.xy), 0).r, 3u, 8u);
    
    FragColor0 = (res << 28u) | 0x8000000u | res; 
}