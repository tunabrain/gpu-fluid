uniform sampler2D D;
uniform sampler2D T;
uniform sampler2D U;
uniform sampler2D V;
uniform sampler2D Pos;

flat in ivec2 vCoord;

out vec4 FragColor0;

const vec2 scale = vec2(1.0/T_WIDTH, 1.0/T_HEIGHT);

void main() {
    vec2 Pos = texelFetch(PPos, vCoord, 0).xy;
    
    float PD = texture(D, Pos*scale - vec2(0.5, 0.5)*scale);
    float PT = texture(D, Pos*scale - vec2(0.5, 0.5)*scale);
    float PU = texture(D, Pos*scale - vec2(0.0, 0.5)*scale);
    float PV = texture(D, Pos*scale - vec2(0.5, 0.0)*scale);
}