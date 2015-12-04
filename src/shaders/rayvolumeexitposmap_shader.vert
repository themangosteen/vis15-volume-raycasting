#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;

// these are interpolated by the gpu and then accessible by same name in fragment shader
out vec3 color;

// uniforms use the same value for all vertices
uniform mat4 modelViewProjMat;

void main()
{
    color = vertexColor;
    gl_Position = modelViewProjMat * vec4(vertexPosition, 1.0);
}
