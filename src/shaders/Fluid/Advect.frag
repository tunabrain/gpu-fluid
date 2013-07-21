uniform sampler2D D;
uniform sampler2D U;
uniform sampler2D V;
uniform float Timestep;
uniform vec2 Offset;
uniform float InvHx;
uniform ivec2 Limits;

in vec2 vCoord;

out float FragColor0;

const vec2 scale  = vec2(1.0/T_WIDTH, 1.0/T_HEIGHT);

vec2 velocity(vec2 pos) {
    return vec2(texture(U, pos*scale + vec2(0.0, -0.5*scale.y)).r, texture(V, pos*scale + vec2(-0.5*scale.x, 0.0)).r)*InvHx;
}

vec2 rungeKutta3(vec2 pos) {
    vec2 first = velocity(pos);
    vec2 midPos = pos - 0.5*Timestep*first;
    vec2 mid = velocity(midPos);
    vec2 lastPos = pos - 0.75*Timestep*mid;
    vec2 last = velocity(lastPos);
    
    return -Timestep*((2.0/9.0)*first + (3.0/9.0)*mid + (4.0/9.0)*last);
}

float cerp(float x, float a, float b, float c, float d) {
    float xsq = x*x;
    float xcu = xsq*x;

    return a*(-0.5*x + xsq - 0.5*xcu)    +
           b*(1.0 - 2.5*xsq + 1.5*xcu)   +
           c*(0.5*x + 2.0*xsq - 1.5*xcu) +
           d*(-0.5*xsq + 0.5*xcu);
}

//void textureWeights(vec2 x, out vec4 g, out vec4 h) {
//    vec2 xsq = x*x;
//    vec2 xcu = xsq*x;
//
//    vec2 w0 = -0.5*x + xsq - 0.5*xcu;
//    vec2 w1 = 1.0 - 2.5*xsq + 1.5*xcu;
//    vec2 w2 = 0.5*x + 2.0*xsq - 1.5*xcu;
//    vec2 w3 = -0.5*xsq + 0.5*xcu;
//    /*vec2 w0 = 1.0/6.0*(-1.0*xcu + 3.0*xsq - 3.0*x + 1.0);
//    vec2 w1 = 1.0/6.0*( 3.0*xcu - 6.0*xsq + 0.0*x + 4.0);
//    vec2 w2 = 1.0/6.0*(-3.0*xcu + 3.0*xsq + 3.0*x + 1.0);
//    vec2 w3 = 1.0/6.0*( 1.0*xcu + 0.0*xsq + 0.0*x + 0.0);*/
//    
//    g.xy = w0 + w1;
//    g.zw = w2 + w3;
//    h.xy = w1/g.xy;
//    h.zw = w3/g.zw;
//}
//
//float textureCerp(vec2 pos) {
//    vec2 p = fract(pos - 0.5);
//    
//    vec4 g, h;
//    textureWeights(p, g, h);
//    
//    pos -= p;
//    
//    vec2 pos00 = pos + vec2(-1.0 + h.x, -1.0 + h.y);
//    vec2 pos10 = pos + vec2( 1.0 + h.z, -1.0 + h.y);
//    vec2 pos01 = pos00 + vec2(0.0, 2.0 - h.y + h.w);
//    vec2 pos11 = pos10 + vec2(0.0, 2.0 - h.y + h.w);
//    
//    float x00 = texture(D, pos00*scale).r;
//    float x01 = texture(D, pos01*scale).r;
//    float x10 = texture(D, pos10*scale).r;
//    float x11 = texture(D, pos11*scale).r;
//    
//    float x0 = x00*g.y + x01*g.w;
//    float x1 = x10*g.y + x11*g.w;
//    
//    return x0*g.x + x1*g.z;
//    
//    //vec2 res = texture(D, (pos - h.xy)*scale).r*g.xy + texture(D, (pos + h.zw)*scale).r*g.zw;
//    
//    
//}

vec4 fetchRow(ivec4 xs, int y) {
    return vec4(
        texelFetch(D, ivec2(xs.x, y), 0).r,
        texelFetch(D, ivec2(xs.y, y), 0).r,
        texelFetch(D, ivec2(xs.z, y), 0).r,
        texelFetch(D, ivec2(xs.w, y), 0).r
    );
}

float textureCerp(vec2 pos) {
    vec2 p = fract(pos - 0.5);
    
    ivec2 iPos = ivec2(pos - 0.5);
    
    ivec4 xs = ivec4(iPos.x - 1, iPos.x, iPos.x + 1, iPos.x + 2);
    ivec4 ys = ivec4(iPos.y - 1, iPos.y, iPos.y + 1, iPos.y + 2);
    
    xs = max(min(xs, Limits.x), 0);
    ys = max(min(ys, Limits.y), 0);

    vec4 c1 = fetchRow(xs, ys.x);
    vec4 c2 = fetchRow(xs, ys.y);
    vec4 c3 = fetchRow(xs, ys.z);
    vec4 c4 = fetchRow(xs, ys.w);
    
    vec4 maxs = max(c1, max(c2, max(c3, c4)));
    vec4 mins = min(c1, min(c2, min(c3, c4)));
    
    float dMax = max(maxs.x, max(maxs.y, max(maxs.z, maxs.w)));
    float dMin = min(mins.x, min(mins.y, min(mins.z, mins.w)));
    
    float v1 = cerp(p.x, c1.x, c1.y, c1.z, c1.w);
    float v2 = cerp(p.x, c2.x, c2.y, c2.z, c2.w);
    float v3 = cerp(p.x, c3.x, c3.y, c3.z, c3.w);
    float v4 = cerp(p.x, c4.x, c4.y, c4.z, c4.w);
    
    //vec4 vs = vec4(v1, v2, v3, v4)*step(vec4(1.0, 0.0, -1.0, -2.0))
    
    //return cerp(p.y, v1, v2, v3, v4);
    
    return clamp(cerp(p.y, v1, v2, v3, v4), dMin, dMax);
}

/*float textureCerp(vec2 pos) {
    //pos = min(pos, vec2(WIDTH - 2.0, HEIGHT - 2.0));
    
    //vec2 p = fract(pos - 0.5);
    vec2 p = fract(pos - 0.5 + 1.0/(512.0 + 64.0 + 32.0 + 4.0)); // #@!*&%+ing nvidia drivers 
    
    pos *= scale;
    
    
    vec4 c1 = textureGatherOffset(D, pos, ivec2(-1, -1));
    vec4 c2 = textureGatherOffset(D, pos, ivec2( 1, -1));
    vec4 c3 = textureGatherOffset(D, pos, ivec2(-1,  1));
    vec4 c4 = textureGatherOffset(D, pos, ivec2( 1,  1));
    
    vec4 maxs = max(c1, max(c2, max(c3, c4)));
    vec4 mins = min(c1, min(c2, min(c3, c4)));
    
    float dMax = max(maxs.x, max(maxs.y, max(maxs.z, maxs.w)));
    float dMin = min(mins.x, min(mins.y, min(mins.z, mins.w)));
    
    float v2 = cerp(p.x, c1.x, c1.y, c2.x, c2.y);
    float v1 = cerp(p.x, c1.w, c1.z, c2.w, c2.z);
    float v4 = cerp(p.x, c3.x, c3.y, c4.x, c4.y);
    float v3 = cerp(p.x, c3.w, c3.z, c4.w, c4.z);
    
    //return cerp(p.y, v1, v2, v3, v4);
    
    return clamp(cerp(p.y, v1, v2, v3, v4), dMin, dMax);
}*/

void main() {
    ivec2 coord = ivec2(vCoord);
    vec2 fCoord = vec2(coord) + 0.5;

    FragColor0 = textureCerp(fCoord + rungeKutta3(fCoord + Offset));
}