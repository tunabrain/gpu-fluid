uniform sampler2D WU;
uniform sampler2D WV;
uniform sampler2D U;
uniform sampler2D V;
uniform float Timestep;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;

void main() {
    ivec2 coord = ivec2(vCoord);
     vec2 scale = vec2(1.0/WIDTH, 1.0/HEIGHT);
     
   float AU = 0.0, AV = 0.0;
   if (coord.x > 1 && coord.y > 1 && coord.x < WIDTH - 2 && coord.y < HEIGHT - 2) {
       AU = texture(WU, (vCoord - vec2(0.5, 0.0))*scale).r;
       AV = texture(WV, (vCoord - vec2(0.0, 0.5))*scale).r;
   }
   
   FragColor0 = texelFetch(U, coord, 0).r + AU*Timestep;
   FragColor1 = texelFetch(V, coord, 0).r + AV*Timestep;
}