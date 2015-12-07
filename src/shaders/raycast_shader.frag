#version 330 core

// interpolated entry position for a ray through the volume
in vec3 entryPos;

// out location 0 is piped directly to the default draw buffer
out vec4 outColor;

// uniforms use the same value for all fragments
uniform sampler1D transferFunction; // maps sampled intensities to color
uniform sampler2D exitPositions; // precalculated exit positions for an orthogonal ray from each fragment
uniform sampler3D volume;
uniform int numSamples;
uniform float sampleRangeStart;
uniform float sampleRangeEnd;
uniform vec2 screenDimensions;
uniform bool alphaTech;
uniform bool avgTech;

void main()
{

    vec3 exitPos = texture2D(exitPositions, gl_FragCoord.st/screenDimensions).xyz;

    if (entryPos == exitPos) {
        discard;
    }

    vec3  ray = exitPos - entryPos;
    float sampleStepSize = length(ray)/numSamples;
    vec3  rayDelta = normalize(ray) * sampleStepSize;
    vec3  currentVoxel = entryPos;

    float intensity;
    float maxIntensity = 0.0;
    vec4  mappedColor; // color mapped to intensity by transferFunction
    vec4 colorAccum = vec4(0.0); // accumulated color from volume traversal
    float alphaAccum = 0.0; // accumulated alpha for blending

    vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 0.0);

    for (int i = 0; i < numSamples; ++i) {

        if (i >= sampleRangeStart*numSamples && i <= sampleRangeEnd*numSamples) {

            intensity = texture3D(volume, currentVoxel).r;
            //outColor = vec4(currentVoxel, 1);

            if (!alphaTech && !avgTech) { // MAXIMUM INTENSITY PROJECTION

                if (intensity > maxIntensity) {
                    maxIntensity = intensity;
                }

            } else if (alphaTech && !avgTech) { // ALPHA COMPOSITING (

                mappedColor = texture1D(transferFunction, intensity);

                // front-to-back integration
                if (mappedColor.a > 0.0) {
                    mappedColor.a = 1.0 - pow(1.0 - mappedColor.a, sampleStepSize*numSamples);
                    colorAccum.rgb += (1.0 - colorAccum.a) * mappedColor.rgb * mappedColor.a;
                    colorAccum.a += (1.0 - colorAccum.a) * mappedColor.a;
                }

                if (colorAccum.a > 1.0) {
                    colorAccum.a = 0.0;
                    break; // terminate if accumulated opacity > 1
                }

            } else if (!alphaTech && avgTech) { // TODO AVERAGE

            }

        }

        currentVoxel += rayDelta;
    }

    if (!alphaTech && !avgTech) { // MAXIMUM INTENSITY PROJECTION
        /*/ <-- toggle
        outColor = texture1D(transferFunction, maxIntensity); /*/
        outColor = vec4(vec3(maxIntensity), 1.0); //*/

    } else if (alphaTech && !avgTech){  // ALPHA COMPOSITING
        outColor = colorAccum;

    } else if (!alphaTech && avgTech) { // TODO AVERAGE

    }

    // DEBUG DRAW FRONT FACES (RAY ENTRY POSITIONS) / BACK FACES (RAY EXIT POSITIONS
    //outColor = vec4(entryPos, 1.0);
    //outColor = vec4(exitPos, 1.0);

}
