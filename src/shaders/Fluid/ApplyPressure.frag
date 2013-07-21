uniform sampler2D U;
uniform sampler2D V;
uniform sampler2D P;

uniform float Scale;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float newU = 0.0, newV = 0.0;
    
    if (coord.x > 0 && coord.x < WIDTH - 1 && coord.y < HEIGHT - 1)
        newU = texelFetch(U, coord, 0).r - Scale*(texelFetch(P, coord, 0).r - texelFetchOffset(P, coord, 0, ivec2(-1, 0)).r);
    if (coord.y > 0 && coord.y < HEIGHT - 1 && coord.x < WIDTH - 1)
        newV = texelFetch(V, coord, 0).r - Scale*(texelFetch(P, coord, 0).r - texelFetchOffset(P, coord, 0, ivec2(0, -1)).r);
    
    FragColor0 = newU;
    FragColor1 = newV;
}