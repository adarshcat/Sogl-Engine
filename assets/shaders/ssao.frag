#version 330 core

#define kernelSize 12

out vec4 FragColor;

in vec2 texCoord;

const vec2 noiseScale = vec2(1024.0/4.0, 768.0/4.0);
const float bias = 0.025;
const float radius = 0.5;

uniform sampler2D gPositionView;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[kernelSize];
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(){
    vec3 worldNormal = texture(gNormal, texCoord).rgb;
    if (length(worldNormal) == 0.0){
        FragColor = vec4(1.0);
        return;
    }

    vec3 viewPos = texture(gPositionView, texCoord).rgb;
    vec3 viewNormal = normalize((viewMatrix*vec4(worldNormal,0.0)).xyz);
    vec3 randomVec = texture(noiseTexture, texCoord * noiseScale).xyz;
    
    vec3 tangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i){
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
        samplePos = viewPos + samplePos * radius;
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projectionMatrix * offset; // convert to clip space
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // clip space to screen space

        float sampleDepth = texture(gPositionView, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));

        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }  

    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = vec4(vec3(occlusion), 1.0);
}