uniform sampler2D D;
uniform sampler2D U;
uniform sampler2D V;
uniform sampler2D P;
uniform sampler2D T;

in vec2 vCoord;

out vec4 FragColor0;

void main() {
    /*double t = abs(texture(T, vCoord, 0).r - 22.0)*1.0/200.0;
    t = clamp(t, 0.0, 1.0);
    
    double r = min(t*4.0, 1.0);
    double g = min(t*2.0, 1.0);
    double b = clamp(t*4.0 - 3.0, 0.0, 1.0);
    
    FragColor0 = vec4(vec3(r, g, b)*texture(D, vCoord, 0).r, 1.0);*/
	FragColor0 = vec4(vec3(1.0 - texture(D, vCoord, 0).r), 1.0);
}