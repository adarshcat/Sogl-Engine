#version 330 core

out vec4 FragColor;

in vec2 texCoord;

struct CameraData {
    mat4 invView;
    mat4 invProjection;
    vec3 position;
    float near;
    float far;
};

struct DirectionalLight {
    sampler2D shadowMap;
    vec3 color;
    vec3 direction;
    float strength;
    mat4 transformMatrix;
};


uniform CameraData camera;
uniform DirectionalLight dirLight;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

const float ssaoBlurCutoff = 0.1;
uniform sampler2D ssaoMap;

#ifdef SHADOW_ENABLED
float shadowCalculation(vec4 fragPosLightSpace, vec3 normal){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(dirLight.shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.0;//max(0.0015 * (1.0 - dot(normal, dirLight.direction)), 0.00035); // 0.0015 // 0.00035

    // shadow caculation with PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirLight.shadowMap, 0);
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(dirLight.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;   
        }    
    }
    shadow /= 25.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}
#endif

float getLinearDepth(float depth){
    float ndc = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * camera.near * camera.far) / (camera.far + camera.near - ndc * (camera.far - camera.near));

    return linearDepth;
}

vec3 getViewpos(vec2 coord, float depth){
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = camera.invProjection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

#ifdef SSAO_ENABLED
float getSSAO(vec2 location, vec3 cameraDirection, vec3 surfaceNormal){
    vec2 texSize = vec2(textureSize(gDepth, 0));
    vec2 texelSize = 1.0 / texSize;
    float result = 0.0;
    float counter = 0;

    float depthSampleCurrent = texture(gDepth, texCoord).r;
    float depthLinearCurrent = getLinearDepth(depthSampleCurrent);

    float dotProd = 1.0 - abs(dot(cameraDirection, surfaceNormal));
    float currentCutoff = ssaoBlurCutoff + 0.5*dotProd;

    for (int x = -1; x < 1; ++x){
        for (int y = -1; y < 1; ++y){
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float depthSample = texture(gDepth, texCoord + offset).r;
            float depthLinear = getLinearDepth(depthSample);
            
            if (abs(depthLinear - depthLinearCurrent) < currentCutoff){
                result += texture(ssaoMap, texCoord + offset).r * ((currentCutoff - abs(depthLinear - depthLinearCurrent)) / currentCutoff);
                counter++;
            }
            
        }
    }

    return 1.0 - result/counter;
}
#endif

vec3 renderSky(vec3 cameraDirection){
    float elevation = 1.0 - max(dot(cameraDirection, vec3(0, 1, 0)), 0.0);
    elevation = pow(elevation, 1.5);

    return mix(vec3(0, 0.9, 1.0), vec3(0.4, 1, 1), elevation);
}

void main(){
    // Retrieve the values from g buffer
    vec3 worldNormal = texture(gNormal, texCoord).rgb;
    vec3 cameraDirection = normalize((camera.invView * vec4((texCoord - vec2(0.5))*2.0, -1, 1)).xyz - camera.position);

    if (length(worldNormal) == 0.0){
        FragColor = vec4(renderSky(cameraDirection), 1.0);
        return;
    }

    vec3 viewPos = getViewpos(texCoord, texture(gDepth, texCoord).r);
    vec3 worldPos = (camera.invView * vec4(viewPos, 1)).xyz;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;

    // phong lighting----------------------------------------------------------------------------------------
    // calculate diffuse
    vec3 diffuse = max(dot(worldNormal, dirLight.direction), 0.0) * dirLight.color * dirLight.strength;

    // ambient lighting with AO
    float ambientStrength = 0.3;
#ifdef SSAO_ENABLED
    float occlusionStrength = getSSAO(texCoord, cameraDirection, worldNormal);
    vec3 ambient = occlusionStrength * ambientStrength * albedo;
#else
    vec3 ambient = ambientStrength * albedo;
#endif

    // calculate specular
    float specularStrength = 0.3;
    vec3 viewDir = normalize(camera.position - worldPos);
    vec3 reflectDir = reflect(-dirLight.direction, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * dirLight.color;

#ifdef SHADOW_ENABLED
    // calculate shadows
    vec4 fragPosLightSpace = dirLight.transformMatrix * vec4(worldPos, 1.0);
    float shadow = shadowCalculation(fragPosLightSpace, worldNormal);
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;
#else
    vec3 lighting = (ambient + (diffuse + specular)) * albedo;
#endif

    // output color with rienhard tonemapping
    vec3 tonemapped = lighting/(lighting+1.0f);
    float gamma = 2.2;
    FragColor = vec4(pow(tonemapped, vec3(1.0/gamma)), 1.0);
}