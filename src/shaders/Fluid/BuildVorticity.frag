uniform sampler2D U;
uniform sampler2D V;
uniform float Scale;

in vec2 vCoord;

out float FragColor0;

void main() {
    ivec2 coord = ivec2(vCoord);
     vec2 scale = vec2(1.0/WIDTH, 1.0/HEIGHT);
     
    float W = 0.0;

    if (coord.x > 1 && coord.y > 1 && coord.x < WIDTH - 2 && coord.y < HEIGHT - 2) {
        float U0 = texture(U, (vCoord + vec2( 0.5, -1.0))*scale).r;
        float U1 = texture(U, (vCoord + vec2( 0.5,  1.0))*scale).r;
        float V0 = texture(V, (vCoord + vec2(-1.0,  0.5))*scale).r;
        float V1 = texture(V, (vCoord + vec2( 1.0,  0.5))*scale).r;
        W  = ((V1 - V0) - (U1 - U0))*Scale;
    }
    
    FragColor0 = W;
}