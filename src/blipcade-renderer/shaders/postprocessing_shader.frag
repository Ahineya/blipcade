in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D TEXTURE;
uniform float TIME;
uniform float SCREEN_WIDTH;
uniform float SCREEN_HEIGHT;

vec4 COLOR;
vec2 SCREEN_UV = gl_FragCoord.xy / vec2(SCREEN_WIDTH, SCREEN_HEIGHT);

%FRAGMENT%

void main()
{
    fragment();

    finalColor = COLOR;
}
