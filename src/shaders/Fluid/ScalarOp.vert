uniform vec4 QuadInfo;
uniform vec4 TexelInfo;
uniform sampler2D Alpha;
uniform sampler2D Beta;

flat out float vAlpha;
out vec2 vCoord;

void main() {
    float A = texture(Alpha, vec2(0.5)).r;
    float B = texture(Beta,  vec2(0.5)).r;
    
    if (abs(B) < 1e-5)
        vAlpha = A*(0.25*1e5);
    else
        vAlpha = A/B;
    
    switch(gl_VertexID) {
    case 0:
        vCoord = TexelInfo.xy;
        gl_Position = vec4(QuadInfo.xy, 0.0, 1.0);
        break;
    case 1:
        vCoord = TexelInfo.xw;
        gl_Position = vec4(QuadInfo.xw, 0.0, 1.0);
        break;
    case 2:
        vCoord = TexelInfo.zw;
        gl_Position = vec4(QuadInfo.zw, 0.0, 1.0);
        break;
    case 3:
        vCoord = TexelInfo.zy;
        gl_Position = vec4(QuadInfo.zy, 0.0, 1.0);
        break;
    }
}