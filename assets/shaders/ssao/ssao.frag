#version 330 core

out vec4 FragColor;

in vec2 texCoord;

const vec2 noiseScale = vec2(WINDOW_WIDTH/4.0, WINDOW_HEIGHT/4.0);
const float bias = 0.08;
const float radius = 0.5;

uniform sampler2D gDepth;
uniform sampler2D gNormalMet;
uniform sampler2D noiseTexture;

uniform vec3 samples[KERNEL_SIZE];

uniform mat4 invProjection;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec3 getViewpos(float depth){
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

void main(){
    vec3 worldNormal = texture(gNormalMet, texCoord).rgb;
    if (length(worldNormal) == 0.0){
        FragColor = vec4(1.0);
        return;
    }

    vec3 viewPos = getViewpos(texture(gDepth, texCoord).r);
    vec3 viewNormal = (viewMatrix*vec4(worldNormal,0.0)).xyz;
    vec3 randomVec = texture(noiseTexture, texCoord * noiseScale).xyz;
    
    vec3 tangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);

    float occlusion = 0.0;
    for(int i = 0; i < KERNEL_SIZE; ++i){
        // get sample position
        vec3 samplePos = TBN * samples[i]; // orient the point along the surface normal
        samplePos = viewPos + samplePos * radius;
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projectionMatrix * offset; // convert to clip space
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // clip space to screen space

        if (offset.x > 1.0 || offset.y > 1.0 || offset.x < 0.0 || offset.y < 0.0) continue;

        float sampleDepth = getViewpos(texture(gDepth, offset.xy).r).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));

        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = occlusion / KERNEL_SIZE;
    FragColor = vec4(occlusion, 0.0, 0.0, 1.0);
}