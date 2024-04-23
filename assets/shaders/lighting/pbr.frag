#version 330 core

#define PI 3.1415926535

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

struct GBuffer {
    sampler2D gDepth;
    sampler2D gNormalMet;
    sampler2D gAlbedoSpec;
};


uniform CameraData camera;
uniform DirectionalLight dirLight;
uniform GBuffer gbuffer;

//ssao
const float ssaoBlurCutoff = 0.1;
uniform sampler2D ssaoMap;

//skybox
uniform samplerCube skyIrradiance;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

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

#ifdef SSAO_ENABLED
float getSSAO(vec2 location){
    vec2 texSize = vec2(textureSize(gbuffer.gDepth, 0));
    vec2 texelSize = 1.0 / texSize;
    float result = 0.0;
    float counter = 0;

    float depthSampleCurrent = texture(gbuffer.gDepth, texCoord).r;
    float depthLinearCurrent = getLinearDepth(depthSampleCurrent);

    float currentCutoff = ssaoBlurCutoff;

    for (int x = -1; x < 1; ++x){
        for (int y = -1; y < 1; ++y){
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float depthSample = texture(gbuffer.gDepth, texCoord + offset).r;
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

// PBR related functions-------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
// PBR functions end----------------------------------------------

void main(){
    // fetch the depth value from depth g buffer and check if its sky------
    float depthValue = texture(gbuffer.gDepth, texCoord).r;

    vec4 normalMetallic = texture(gbuffer.gNormalMet, texCoord).rgba;
    vec3 worldNormal = normalize(normalMetallic.rgb);
    //vec3 cameraDirection = normalize((camera.invView * vec4((texCoord - vec2(0.5))*2.0, -1, 1)).xyz - camera.position);

    if (depthValue >= 1.0){
        discard;
    }

    vec3 viewPos = getViewpos(texCoord, depthValue);
    vec3 worldPos = (camera.invView * vec4(viewPos, 1)).xyz;

    vec3 viewDir = normalize(camera.position - worldPos);

    vec4 albedoSpecData = texture(gbuffer.gAlbedoSpec, texCoord).rgba;
    vec3 albedo = albedoSpecData.rgb;
    float roughness = albedoSpecData.a;
    float metallic = normalMetallic.a;
    // g-buffer fetch/data-reconstruction complete---------------------------

    // PBR

    vec3 halfVector = normalize(viewDir + dirLight.direction);
    vec3 reflectionVector = reflect(viewDir, worldNormal);

    vec3 radiance = dirLight.color * dirLight.strength;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 F = fresnelSchlick(max(dot(halfVector, viewDir), 0.0), F0);
    float NDF = DistributionGGX(worldNormal, halfVector, roughness);
    float G = GeometrySmith(worldNormal, viewDir, dirLight.direction, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(worldNormal, viewDir), 0.0) * max(dot(worldNormal, dirLight.direction), 0.0)  + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
  
    float NdotL = max(dot(worldNormal, dirLight.direction), 0.0);
    vec3 lightOutput = (kD * albedo / PI + specular) * radiance * NdotL;


    // ssao
#ifdef SSAO_ENABLED
    float ao = getSSAO(texCoord);
#else
    float ao = 1.0;
#endif


#ifdef IRRADIANCE_ENABLED
    // fresnel calculation for indirect lighting from the scene
    F = fresnelSchlickRoughness(max(dot(worldNormal, viewDir), 0.0), F0, roughness);
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    // getting diffuse irradiance
    vec3 diffuseIrradiance = texture(skyIrradiance, worldNormal).rgb;
    vec3 diffuse = diffuseIrradiance * albedo;

    // calculating specular irradiance
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectionVector,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(worldNormal, viewDir), 0.0), roughness)).rg;
    vec3 envSpecular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + envSpecular) * ao;
#else
    vec3 ambient = kD * vec3(0.2) * albedo * ao;
#endif

    // shadows
#ifdef SHADOW_ENABLED
    vec4 fragPosLightSpace = dirLight.transformMatrix * vec4(worldPos, 1.0);
    float shadow = shadowCalculation(fragPosLightSpace, worldNormal);
    
    vec3 lighting = ambient + (1.0 - shadow) * lightOutput;
#else
    vec3 lighting = ambient + lightOutput;
#endif


    // tonemapping and gamma correction
    vec3 tonemapped = lighting/(lighting+1.0f);
    float gamma = 2.2;
    FragColor = vec4(pow(tonemapped, vec3(1.0/gamma)), 1.0);
}