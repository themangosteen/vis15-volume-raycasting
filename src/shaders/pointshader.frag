#version 330 core

in vec3 P;
in float intensity;

// out location 0 is piped directly to the default draw buffer
out vec4 outColor;

uniform vec4 color;

void main()
{
    outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f) * intensity;
}
