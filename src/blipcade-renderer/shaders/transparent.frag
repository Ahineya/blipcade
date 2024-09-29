#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 transparentColor; // The color to be treated as transparent

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord) * colDiffuse;

    // Use a small threshold to account for floating-point inaccuracies
    float threshold = 0.01;
    if (distance(texelColor.rgb, transparentColor.rgb) < threshold)
    {
        discard;
    }

    finalColor = texelColor;
}