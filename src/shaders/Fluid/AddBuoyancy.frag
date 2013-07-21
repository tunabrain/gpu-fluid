uniform sampler2D T;
uniform sampler2D V;
uniform float Timestep;
uniform float G;
uniform float Density;
uniform float TAmb;

in vec2 vCoord;

out float FragColor0;

void main() {
    ivec2 coord = ivec2(vCoord);
     vec2 scale = vec2(1.0/WIDTH, 1.0/HEIGHT);
     
   float TV = texture(T, (vCoord - vec2(0.0, 0.5))*scale).r;
   float BV = -Density/TAmb*(TV - TAmb)*G;
   
   if (coord.x < 1 || coord.y < 1 || coord.x > WIDTH - 2 || coord.y > WIDTH - 2)
       BV = 0.0;
   
   FragColor0 = texelFetch(V, coord, 0).r + BV*Timestep;
}