uniform sampler2D Source;
uniform float Value;
uniform float ValueMin;

layout(pixel_center_integer) in vec4 gl_FragCoord;
in vec2 vCoord;

out float FragColor0;

void main() {
    vec2 W = 1.0 - abs(1.0 - vCoord);
    float Current = texelFetch(Source, ivec2(gl_FragCoord.xy), 0).r;
    float New = ValueMin + Value*W.x*W.y;
    if (abs(New) > abs(Current))
    	FragColor0 = New;
    else
    	FragColor0 = Current;
}