layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform float uSquareSize;
uniform float uWindowAspectRatio;

void main() {
    vec2 pos = aPos;
    if (uWindowAspectRatio > 1.0) {
        pos.x /= uWindowAspectRatio;
    } else {
        pos.y *= uWindowAspectRatio;
    }
    gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
