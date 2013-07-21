uniform sampler2D W;
uniform float Scale;
uniform float Hx;
uniform float Epsilon;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
 
    vec2 M = vec2(0.0);
    if (coord.x > 1 && coord.y > 1 && coord.x < WIDTH - 2 && coord.y < HEIGHT - 2) {
        float W0 = texelFetch      (W, coord, 0               ).r;
        float U0 = texelFetchOffset(W, coord, 0, ivec2(-1,  0)).r;
        float U1 = texelFetchOffset(W, coord, 0, ivec2( 1,  0)).r;
        float V0 = texelFetchOffset(W, coord, 0, ivec2( 0, -1)).r;
        float V1 = texelFetchOffset(W, coord, 0, ivec2( 0,  1)).r;
        
        vec3 N = vec3(abs(U1) - abs(U0), abs(V1) - abs(V0), 0.0)*Scale;
        N = N/(length(N) + 1e-10);
        vec3 W = vec3(0.0, 0.0, W0);
        M = Epsilon*Hx*(cross(N, W).xy);
    }
    
    FragColor0 = M.x;
    FragColor1 = M.y;
}