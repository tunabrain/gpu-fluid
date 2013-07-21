uniform float Scale;

in vec2 vCoord;

out float FragColor0;
out float FragColor1;
out float FragColor2;

void main() {
    ivec2 coord = ivec2(vCoord);
    
    float D = 0.0, PX = 0.0, PY = 0.0;

    if (fluidCell(coord + ivec2(1, 0))) {
        PX = -Scale;
        D += Scale;
    }
    if (fluidCell(coord + ivec2(0, 1))) {
        PY = -Scale;
        D += Scale;
    }
    if (fluidCell(coord + ivec2(0, -1)))
        D += Scale;
    if (fluidCell(coord + ivec2(-1, 0)))
        D += Scale;
    
    FragColor0 = D;
    FragColor1 = PX;
    FragColor2 = PY;
}