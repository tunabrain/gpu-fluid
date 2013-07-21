uniform vec4 QuadInfo;

void main() {
    switch(gl_VertexID) {
    case 0:
        gl_Position = vec4(QuadInfo.xy, 0.0, 1.0);
        break;
    case 1:
        gl_Position = vec4(QuadInfo.xw, 0.0, 1.0);
        break;
    case 2:
        gl_Position = vec4(QuadInfo.zw, 0.0, 1.0);
        break;
    case 3:
        gl_Position = vec4(QuadInfo.zy, 0.0, 1.0);
        break;
    }
}