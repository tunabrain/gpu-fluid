uniform usampler2D Counts;
uniform usampler2D Offsets;

uniform sampler2D PPos;
uniform sampler2D Q;

uniform ivec2 PointInfo;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out float FragColor0;
out float FragColor1;
out float FragColor2;
out float FragColor3;

const float marker = uintBitsToFloat(0xDEADBEEFu);

void evalContribution(ivec2 iCoord, inout vec4 C, inout float W) {
    if (iCoord.x < 0 || iCoord.y < 0 || iCoord.x > WIDTH - 1 || iCoord.y > HEIGHT - 1)
        return;
    
    int offset = int(texelFetch(Offsets, iCoord, 0).r);
    int count  = int(texelFetch( Counts, iCoord, 0).r >> uint(28));
    
    for (int i = 0; i < count; i++) {
        ivec2 coord = ivec2((offset + i) % PointInfo.x, (offset + i)/PointInfo.x);
        vec4 qs = texelFetch(Q, coord, 0);
        
        if (qs.r != marker) {
            vec2 pos = texelFetch(PPos, coord, 0).xy;
            vec2 d = max(1.0 - abs(pos - gl_FragCoord.xy), 0.0);
            
            W += d.x*d.y;
            C += qs*d.x*d.y;
        }
    }
}

void main() {
    ivec2 iCoord = ivec2(gl_FragCoord.xy);
    
    vec4 C = vec4(0.0);
    float W = 0.0;
    evalContribution(iCoord + ivec2(-1, -1), C, W);
    evalContribution(iCoord + ivec2( 0, -1), C, W);
    evalContribution(iCoord + ivec2( 1, -1), C, W);
    evalContribution(iCoord + ivec2(-1,  0), C, W);
    evalContribution(iCoord + ivec2( 0,  0), C, W);
    evalContribution(iCoord + ivec2( 1,  0), C, W);
    evalContribution(iCoord + ivec2(-1,  1), C, W);
    evalContribution(iCoord + ivec2( 0,  1), C, W);
    evalContribution(iCoord + ivec2( 1,  1), C, W);
    
    if (W == 0.0)
        FragColor0 = FragColor1 = FragColor2 = FragColor3 = marker;
    else {
        FragColor0 = C.x/W;
        FragColor1 = C.y/W;
        FragColor2 = C.z/W;
        FragColor3 = C.w/W;
    }
}