#version 330 core

in vec3 P;
in float intensity;

// out location 0 is piped directly to the default draw buffer
out vec4 outColor;

uniform vec4 color;

void main()
{
    outColor = vec4(intensity+0.5, intensity+0.5, intensity+0.5, 0.1f);
}
