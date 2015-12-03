#version 330 core

in vec4 vertexData;

// these are interpolated by the gpu and then accessible by same name in fragment shader
out vec3 P;
out float intensity;

// uniforms use the same value for all vertices
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{
    intensity = vertexData.w;
    P = (modelMat * vec4(vertexData.xyz, 1)).xyz;

    gl_PointSize = max(500/-(viewMat * vec4(P, 1)).z, 2);
    gl_Position = projMat * viewMat * modelMat * vec4(vertexData.xyz, 1);
}
