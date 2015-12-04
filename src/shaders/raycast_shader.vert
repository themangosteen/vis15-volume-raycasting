#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;

// entry positions for rays through the volume
// these will be interpolated by the gpu and then accessible by same name in fragment shader
// since we also interpolate exit positions in a different shader (sampled from a texture in fragment shader),
// we get all the possible rays (entry-exit pairs) through the volume
out vec3 entryPos;

// the exit position in world coordinates
out vec4 exitPosWorld;

// uniforms use the same value for all vertices
uniform mat4 modelViewProjMat;

void main()
{
    entryPos = vertexColor; // TODO EXPLAIN

    gl_Position = modelViewProjMat * vec4(vertexPosition, 1.0);
    exitPosWorld = gl_Position; // TODO EXPLAIN
}
