uniform sampler2D U;
uniform sampler2D V;
uniform float InvHx;

in vec2 vCoord;

out float FragColor0;

void main() {
    ivec2 coord = ivec2(vCoord);

    float divU = texelFetchOffset(U, coord, 0, ivec2(1, 0)).r - texelFetch(U, coord, 0).r;
    float divV = texelFetchOffset(V, coord, 0, ivec2(0, 1)).r - texelFetch(V, coord, 0).r;
    FragColor0 = -InvHx*(divU + divV);
}