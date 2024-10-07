in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D baseTexture;
uniform sampler2D maskTexture;      // Mask defining the lighting area
uniform vec4 tintColor;             // The tint color (RGBA)
uniform float opacity;              // Opacity of the tint

float blendOverlay(float base, float blend) {
    return base<0.5?(2.0*base*blend):(1.0-2.0*(1.0-base)*(1.0-blend));
}

vec3 blendOverlay(vec3 base, vec3 blend) {
    return vec3(blendOverlay(base.r,blend.r),blendOverlay(base.g,blend.g),blendOverlay(base.b,blend.b));
}

vec3 blendOverlay(vec3 base, vec3 blend, float opacity) {
    return (blendOverlay(base, blend) * opacity + base * (1.0 - opacity));
}

void main()
{
    vec4 baseColor = texture(baseTexture, fragTexCoord);
    float mask = texture(maskTexture, fragTexCoord).r; // Assuming mask is grayscale

    vec3 tintedColor = blendOverlay(baseColor.rgb, tintColor.rgb, mask * opacity);

    finalColor = vec4(tintedColor, baseColor.a);
}
