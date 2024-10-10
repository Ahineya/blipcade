float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 bloom(float amount, vec2 uv) {
    vec3 bloom = vec3(0.0);

    for (int i = 0; i < 9; i++) {
        vec2 offset = vec2(float(i % 3) - 1.0, float(i / 3) - 1.0) * 0.004;
        bloom += textureLod(TEXTURE, uv + offset, 0.0).rgb;
    }

    bloom /= 9.0;

    return bloom * amount;
}

float vignette(float amount, vec2 uv) {
    float vignette = 1.0 - pow(length(uv - 0.5) * 1.5, 3.0);
    return vignette * amount + (1.0 - amount);
}

float grain(float amount, vec2 uv) {
    return (rand(uv) - 0.5) * amount;
}

float scanlines(float amount) {
    return (1.0 - mod(gl_FragCoord.y, 2.0) * 0.1) * amount + (1.0 - amount);
}

vec3 crt() {
    vec2 crt = SCREEN_UV;
    crt.x = pow(crt.x, 1.1);
    crt.y = pow(crt.y, 1.1);
    crt = crt * 2.0 - 1.0;
    crt *= 1.0 + 0.05 * sin(gl_FragCoord.y * 0.1);
    crt = (crt + 1.0) * 0.5;

    crt.y = 1.0 - crt.y;

    float crtArea = 1.0 - pow(length(SCREEN_UV - 0.5) * 1.5, 2.0);

    return mix(COLOR.rgb, texture(TEXTURE, crt).rgb, (1.0 - crtArea) * 0.05);
}

vec3 dithering(vec3 color) {
    float grid_position = mod(gl_FragCoord.x + gl_FragCoord.y, 2.0);
    vec3 dither = vec3(1.0) * grid_position;
    return color + dither;
}

vec2 curveScreen(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    float radius = 1.0;
    float dist = length(uv);
    if (dist < radius) {
        float z = sqrt(radius * radius - dist * dist);
        float factor = z / radius;
        uv *= factor;
    }
    return uv * 0.5 + 0.5;
}

vec3 phosphorGlow(vec2 uv) {
    float glow = exp(-mod(TIME * 24.0, 1.0) * 2.0);
    vec3 color = texture(TEXTURE, uv).rgb * glow;
    return color;
}

vec3 signalNoise(vec2 uv) {
    float noise = rand(vec2(TIME * 0.05, uv.y)) * 0.02;
    return vec3(noise);
}

vec2 curveScreen(vec2 uv, float amount) {
    uv = uv * 2.0 - 1.0;// Transform from [0,1] to [-1,1]

    // Apply barrel distortion formula
    uv *= vec2(
    sqrt(1.0 - amount * uv.y * uv.y),
    sqrt(1.0 - amount * uv.x * uv.x)
    );

    uv = (uv + 1.0) * 0.5;// Transform back to [0,1]
    return uv;
}

//float edgeDetection(vec2 uv) {
//    float texelSizeX = 1.0 / SCREEN_WIDTH;
//    float texelSizeY = 1.0 / SCREEN_HEIGHT;
//
//    vec3 colorCenter = texture(TEXTURE, uv).rgb;
//    vec3 colorRight = texture(TEXTURE, uv + vec2(texelSizeX, 0.0)).rgb;
//    vec3 colorUp = texture(TEXTURE, uv + vec2(0.0, texelSizeY)).rgb;
//
//    float lumCenter = dot(colorCenter, vec3(0.299, 0.587, 0.114));
//    float lumRight = dot(colorRight, vec3(0.299, 0.587, 0.114));
//    float lumUp = dot(colorUp, vec3(0.299, 0.587, 0.114));
//
//    float dx = lumCenter - lumRight;
//    float dy = lumCenter - lumUp;
//
//    float edge = sqrt(dx * dx + dy * dy);
//
//    edge = clamp(edge * 10.0, 0.0, 1.0);
//
//    return edge;
//}

float edgeDetection(vec2 uv, float amount) {
    float texelSizeX = 1.0 / SCREEN_WIDTH;
    float texelSizeY = 1.0 / SCREEN_HEIGHT;

    // Initialize luminance values
    float lum[25];

    // Sample neighboring pixels and compute luminance
    int index = 0;
    for (int j = -2; j <= 2; j++) {
        for (int i = -2; i <= 2; i++) {
            vec2 offset = vec2(float(i) * texelSizeX, float(j) * texelSizeY);
            vec3 color = texture(TEXTURE, uv + offset).rgb;
            lum[index++] = dot(color, vec3(0.299, 0.587, 0.114));
        }
    }

    // Define 5x5 Sobel kernels
    float kernelX[25] = float[](
    -2, -1,  0,  1,  2,
    -2, -1,  0,  1,  2,
    -4, -2,  0,  2,  4,
    -2, -1,  0,  1,  2,
    -2, -1,  0,  1,  2
    );

    float kernelY[25] = float[](
    -2, -2, -4, -2, -2,
    -1, -1, -2, -1, -1,
    0,  0,  0,  0,  0,
    1,  1,  2,  1,  1,
    2,  2,  4,  2,  2
    );

    // Apply Sobel operator
    float gx = 0.0;
    float gy = 0.0;
    for (int i = 0; i < 25; i++) {
        gx += lum[i] * kernelX[i];
        gy += lum[i] * kernelY[i];
    }

    // Compute edge magnitude
    float edge = sqrt(gx * gx + gy * gy);

    // Normalize and clamp edge value
    edge = clamp(edge * amount, 0.0, 1.0);

    return edge;
}

vec3 dithering(vec3 color, float edgeMask, float amount) {
    // Generate a Bayer matrix pattern for ordered dithering
    int x = int(mod(gl_FragCoord.x, 4.0));
    int y = int(mod(gl_FragCoord.y, 4.0));

    // Flattened 4x4 Bayer matrix values normalized between 0 and 1
    float bayer[16] = float[](
    0.0 / 16.0, 8.0 / 16.0, 2.0 / 16.0, 10.0 / 16.0,
    12.0 / 16.0, 4.0 / 16.0, 14.0 / 16.0, 6.0 / 16.0,
    3.0 / 16.0, 11.0 / 16.0, 1.0 / 16.0, 9.0 / 16.0,
    15.0 / 16.0, 7.0 / 16.0, 13.0 / 16.0, 5.0 / 16.0
    );

    // Calculate the index into the flattened Bayer matrix
    int index = y * 4 + x;

    // Get threshold from Bayer matrix
    float threshold = bayer[index];

    // Dithering amount (adjust as needed)
    float ditherAmount = amount * edgeMask;

    // Apply dithering
    color += ditherAmount * (threshold - 0.5);

    return color;
}

void fragment() {
    vec2 uv = fragTexCoord;

//    uv = curveScreen(uv, -0.05);

    COLOR = texture(TEXTURE, uv);

    // Edge detection
    float edgeMask = edgeDetection(uv, 0.7); // Higher value — wider edges
    COLOR.rgb = dithering(COLOR.rgb, edgeMask, 0.12);

    COLOR.rgb += signalNoise(uv);

    COLOR.rgb += grain(0.03, uv);

    COLOR.rgb *= vignette(0.5, uv);
    COLOR.rgb *= scanlines(0.5);

    float speed = 30.0;
    float scanlineY = mod(TIME * speed, 240.0);
    float scanlineAlpha = 0.03;
    float scanlineWidth = 1.0;
    float scanlineColor = 1.0;

    if (gl_FragCoord.y > scanlineY && gl_FragCoord.y < scanlineY + scanlineWidth) {
        COLOR.rgb = mix(COLOR.rgb, vec3(scanlineColor), scanlineAlpha);
    }

    COLOR.rgb += bloom(0.1, uv);

    // Color grading
    COLOR.rgb = mix(COLOR.rgb, vec3(0.7, 0.9, 0.8), 0.05);
//
//    // Contrast
    vec3 contrast = (COLOR.rgb - 0.5) * 2.0 + 0.5;
    COLOR.rgb = mix(COLOR.rgb, contrast, 0.05);
//
    COLOR.rgb = pow(COLOR.rgb, vec3(1.2));

//    COLOR.rgb = vec3(edgeMask);
//
//    COLOR.rgb += phosphorGlow(fragTexCoord) * 0.02;
}