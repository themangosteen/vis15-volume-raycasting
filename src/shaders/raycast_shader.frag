#version 330 core

// interpolated entry position for a ray through the volume
in vec3 entryPos;

// out location 0 is piped directly to the default draw buffer
out vec4 outColor;

// uniforms use the same value for all fragments
uniform sampler1D transferFunction; // maps sampled intensities to color
uniform sampler2D exitPositions; // precalculated exit positions for an orthogonal ray from each fragment
uniform sampler3D volume;
//uniform sampler3D gradients; // directions of greatest change at each voxel, used as normals for shading
uniform int numSamples; // number of samples along each ray
uniform float sampleRangeStart;
uniform float sampleRangeEnd;
uniform vec2 screenDimensions;

// COMPOSITING METHODS
// 0: Maximum Intensity Projection
// 1: Average Intensity Projection
// 2: Alpha compositing
uniform int compositingMethod;
uniform bool enableShading;

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

    // Shading
    vec3  normal = vec3(0);
    vec3  view = normalize(ray);
    vec3  firstHitPos;

    vec3 lightPos = vec3(5, 5, 5);
    vec3 lightAmb = vec3(0.3, 0.3, 0.3);
    vec3 lightDif = vec3(0.7, 0.7, 0.7);
    vec3 lightSpec = vec3(1.0, 1.0, 1.0);

    float intensity;
    float maxIntensity = 0.0;
    float intensityAccum = 0.0;
    float intensityCount = 0.0;
    vec4  mappedColor; // color mapped to intensity by transferFunction
    vec4  colorAccum = vec4(0.0); // accumulated color from volume traversal

    vec4 backgroundColor = vec4(1.0, 1.0, 1.0, 0.0);

    for (int i = 0; i < numSamples; ++i) {

        if (i >= sampleRangeStart * numSamples && i <= sampleRangeEnd * numSamples) {

            intensity = texture3D(volume, currentVoxel).r;

            if (normal == vec3(0) && intensity > 0.3) {
                //normal = normalize(texture3D(gradients, currentVoxel).rgb);
                firstHitPos = currentVoxel;
            }

            if (compositingMethod == 0) { // MAXIMUM INTENSITY PROJECTION

                if (intensity > maxIntensity) {
                    maxIntensity = intensity;
                }

            } else if (compositingMethod == 1) { // AVERAGE INTENSITY PROJECTION
                intensityAccum += intensity;
                if (intensity > 0.0) {
                    intensityCount += 1;
                }

            } else if (compositingMethod == 2) { // ALPHA COMPOSITING (

                mappedColor = texture1D(transferFunction, intensity);
                mappedColor.a = intensity;

                colorAccum += mappedColor * mappedColor.a * (1 - colorAccum.a);

                if (colorAccum.a > 1.0) {
                    colorAccum.a = 1.0;
                    break; // terminate if accumulated opacity > 1
                }
            }

        }

        currentVoxel += rayDelta;
    }

    if (compositingMethod == 0) { // MAXIMUM INTENSITY PROJECTION
        // <-- toggle
        outColor = texture1D(transferFunction, maxIntensity); /*/
        outColor = vec4(vec3(maxIntensity), 1.0); //*/

    } else if (compositingMethod == 1) { // AVERAGE INTENSITY PROJECTION
        //float numSamplesFl = 1.0 * numSamples;
        float avgIntensity = intensityAccum / intensityCount;
        if (avgIntensity > 1.0) { avgIntensity = 1.0; }
        outColor = texture1D(transferFunction, avgIntensity);

    } else if (compositingMethod == 2) {  // ALPHA COMPOSITING
        outColor = colorAccum;
    }

    // BLINN PHONG ILLUMINATION MODEL
    if (enableShading) {
        vec3 gradient;
        gradient.x = texture3D(volume, vec3(firstHitPos.x+sampleStepSize, firstHitPos.yz)).r - texture3D(volume, vec3(firstHitPos.x-sampleStepSize, firstHitPos.yz)).r;
        gradient.y = texture3D(volume, vec3(firstHitPos.x, firstHitPos.y+sampleStepSize, firstHitPos.z)).r - texture3D(volume, vec3(firstHitPos.x, firstHitPos.y-sampleStepSize, firstHitPos.z)).r;
        gradient.z = texture3D(volume, vec3(firstHitPos.xy, firstHitPos.z+sampleStepSize)).r - texture3D(volume, vec3(firstHitPos.xy, firstHitPos.z-sampleStepSize)).r;
        normal = normalize(gradient);
        vec3  surfaceColor = outColor.rgb;
        float surfaceAlpha = outColor.a;
        vec3  lightDir = normalize(lightPos - firstHitPos);
        vec3  ambient = lightAmb * surfaceColor;
        vec3  diffuse = max(dot(normal, lightDir), 0.0f) * surfaceColor * lightDif;
        float shininess = 0.05;

        vec3 halfVec = normalize(lightDir + view); // half vector of light and view vectors
        vec3 specular = pow(max(dot(halfVec, normal), 0.0f), shininess) * lightSpec * lightSpec;

        outColor = vec4(vec3(ambient + diffuse + specular), surfaceAlpha);
    }

    // DEBUG DRAW FRONT FACES (RAY ENTRY POSITIONS) / BACK FACES (RAY EXIT POSITIONS
    //outColor = vec4(entryPos, 1.0);
    //outColor = vec4(exitPos, 1.0);

}
