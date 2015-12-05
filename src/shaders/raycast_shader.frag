#version 330 core

// interpolated entry position for a ray through the volume
in vec3 entryPos;

// interpolated exit position in world space
// this can be used for better sampling of the exitPositions texture
in vec4 exitPosWorld;

// out location 0 is piped directly to the default draw buffer
out vec4 outColor;

// uniforms use the same value for all fragments
uniform sampler1D transferFunction; // maps sampled intensities to color
uniform sampler2D exitPositions; // precalculated exit positions for an orthogonal ray from each fragment
uniform sampler3D volume;
uniform float rayStepSize;
uniform vec2 screenDimensions;

void main()
{

    // determine ray exit position (sample exitPositions texture)
    vec3 exitPos = texture2D(exitPositions, gl_FragCoord.st/screenDimensions).xyz;

    // alternative sampling of the exitPositions texture using exitPosWorld
    // TODO WHAT ABOUT THIS?
    // quote: that will actually give you clip-space coordinates rather than
    // normalised device coordinates, since you're not performing the perspective
    // division which happens during the rasterisation process (between the vertex
    // shader and fragment shader
    //vec2 exitFragCoord = (exitPosWorld.xy / exitPosWorld.w + 1.0)/2.0;
    //vec3 exitPos  = texture(exitPositions, exitFragCoord).xyz;

    // TODO EXPLAIN
    if (entryPos == exitPos) {
        discard; // this is the case for background
    }

    vec3  ray = exitPos - entryPos;
    float rayDistance = length(ray); // the length from front to back is calculated and used to terminate the ray
    vec3  rayDelta = normalize(ray) * rayStepSize;
    float rayDeltaDistance = length(rayDelta);
    vec3  currentVoxel = entryPos;

    vec4 colorAccum = vec4(0.0); // accumulated color from volume traversal
    float alphaAccum = 0.0; // accumulated alpha for blending

    float intensity;
    vec4  mappedColor; // color mapped to intensity by transferFunction
    float distanceTraversed = 0.0; // distance traversed from entryPos

    vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 0.0);

    // number of steps is arbitrary due to termination criteria
    // but should be sufficient to traverse all practical volumes
    for (int i = 0; i < 1600; ++i) {

        intensity = texture3D(volume, currentVoxel).x;
        mappedColor = texture1D(transferFunction, intensity);

        // TODO CHANGE TO MAXIMUM INTENSITY PROJECTION
        // REUSE ALPHA COMPOSITING CODE LATER (SWITCH BETWEEN MODES VIA UNIFORM INT)
        // modulate the value of colorSample.a
        // front-to-back integration
        if (mappedColor.a > 0.0) {
            // accomodate for variable sampling rates (base interval defined by mod_compositing.frag)
            mappedColor.a = 1.0 - pow(1.0 - mappedColor.a, rayStepSize*200.0f);
            colorAccum.rgb += (1.0 - colorAccum.a) * mappedColor.rgb * mappedColor.a;
            colorAccum.a += (1.0 - colorAccum.a) * mappedColor.a;
        }

        currentVoxel += rayDelta;
        distanceTraversed += rayDeltaDistance;

        if (distanceTraversed >= rayDistance) {
            colorAccum.rgb = colorAccum.rgb*colorAccum.a + (1 - colorAccum.a)*backgroundColor.rgb;
            break; // terminate if ray has already left the volume, and assign background color
        }
        else if (colorAccum.a > 1.0) {
            colorAccum.a = 1.0;
            break; // terminate if accumulated opacity > 1
        }
    }

    outColor = colorAccum;
    //outColor = vec4(entryPos, 1.0);
    //outColor = vec4(exitPos, 1.0);

}
