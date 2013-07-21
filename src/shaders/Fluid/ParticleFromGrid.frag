uniform sampler2D PPos;
uniform sampler2D Q;
uniform sampler2D D;
uniform sampler2D T;
uniform sampler2D U;
uniform sampler2D V;
uniform sampler2D UOld;
uniform sampler2D VOld;
uniform sampler2D TOld;
uniform sampler2D DOld;
uniform ivec2 PointInfo;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out vec4 FragColor0;

const vec2 scale = vec2(1.0/T_WIDTH, 1.0/T_HEIGHT);

void main() {
    const float marker = uintBitsToFloat(0xDEADBEEFu);
    
    ivec2 coord = ivec2(gl_FragCoord.xy);
    int index = coord.x + coord.y*PointInfo.x;
    if (index >= PointInfo.y)
        discard;
    
    vec2 pos = texelFetch(PPos, coord, 0).xy + 0.5;
    vec4 qs  = texelFetch(Q,    coord, 0);
    
    vec2 posD = min(pos - vec2(0.5, 0.5)*0, vec2(WIDTH - 1.5, HEIGHT - 1.5))*scale;
    vec2 posT = min(pos - vec2(0.5, 0.5)*0, vec2(WIDTH - 1.5, HEIGHT - 1.5))*scale;
    vec2 posU = (pos - vec2(0.0, 0.5)*0)*scale;
    vec2 posV = (pos - vec2(0.5, 0.0)*0)*scale;
    
    if (qs.x == marker)
        FragColor0 = vec4(
            texture(D, posD).r,
            texture(T, posT).r,
            texture(U, posU).r,
            texture(V, posV).r
        );
    else {
        float dDiff = texture(D, posD).r - texture(DOld, posD).r;
        float tDiff = texture(T, posT).r - texture(TOld, posT).r;
        float uDiff = texture(U, posU).r - texture(UOld, posU).r;
        float vDiff = texture(V, posV).r - texture(VOld, posV).r;
        FragColor0 = qs + vec4(dDiff, tDiff, uDiff, vDiff);
    }
}