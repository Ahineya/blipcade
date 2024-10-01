in vec3 vertexPosition;
in vec2 vertexTexCoord;

// Uniforms
uniform mat4 mvp; // Model-View-Projection matrix

// Outputs
out vec2 fragTexCoord;

void main()
{
    fragTexCoord = vertexTexCoord;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}