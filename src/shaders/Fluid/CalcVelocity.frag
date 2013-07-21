uniform sampler2D U;
uniform sampler2D V;

in vec2 vCoord;

out float FragColor0;

void main() {
    vec2 scale = vec2(1.0/WIDTH, 1.0/HEIGHT);
     
    float u = texture(U, (vCoord + vec2(0.5, 0.0))*scale).r;
    float v = texture(V, (vCoord + vec2(0.0, 0.5))*scale).r;

    FragColor0 = length(vec2(u, v));
}