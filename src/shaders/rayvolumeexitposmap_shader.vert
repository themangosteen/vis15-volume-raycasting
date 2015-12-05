#version 330 core

in vec3 vertexPosition;

// out attributes are interpolated for each fragment and then accessible in fragment shader
// here we store vertex positions in color information to interpolate
// between all possible positions where a ray might leave the volume
// note that obviously we want only the vertices of volume cube backfaces, thus front face culling must be enabled
out vec3 color;

// uniforms use the same value for all vertices
uniform mat4 modelViewProjMat;

void main()
{
    color = vertexPosition; // store volume cube backface vertex positions in color information
    gl_Position = modelViewProjMat * vec4(vertexPosition, 1.0);
}
