uniform sampler2D PPos;
uniform sampler2D U;
uniform sampler2D V;
uniform ivec2 PointInfo;
uniform float InvHx;
uniform float Timestep;

layout(pixel_center_integer) in vec4 gl_FragCoord;

out vec2 FragColor0;

const vec2 scale = vec2(1.0/T_WIDTH, 1.0/T_HEIGHT);

vec2 velocity(vec2 pos) {
    return vec2(texture(U, pos*scale + vec2(0.5*scale.x, 0.0)).r, texture(V, pos*scale + vec2(0.0, 0.5*scale.y)).r)*InvHx;
}

vec2 rungeKutta3(vec2 pos) {
    vec2 first = velocity(pos);
    vec2 midPos = pos + 0.5*Timestep*first;
    vec2 mid = velocity(midPos);
    vec2 lastPos = pos + 0.75*Timestep*mid;
    vec2 last = velocity(lastPos);
    
    return Timestep*((2.0/9.0)*first + (3.0/9.0)*mid + (4.0/9.0)*last);
}

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    int index = coord.x + coord.y*PointInfo.x;
    if (index >= PointInfo.y)
        discard;
    
    vec2 pos = texelFetch(PPos, coord, 0).xy;
    pos += rungeKutta3(pos);
    
    FragColor0 = clamp(pos, vec2(0.0), vec2(WIDTH - 1.0001, HEIGHT - 1.0001));
}