in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 palette[256];

void main()
{
    float index = texture(texture0, fragTexCoord).r; // Read palette index (normalized 0.0 - 1.0)
    int paletteIndex = int(index * 255.0 + 0.5);     // Convert to integer index [0, 255]

    finalColor = palette[paletteIndex];
}