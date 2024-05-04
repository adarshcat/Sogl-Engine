#version 330 core

#define PI 3.1415926535

out vec4 fragColor;

in vec4 clipSpacePos;
in vec2 texCoord;
in vec3 viewPos;
in vec3 worldNormal;

uniform sampler2D gDepth;

uniform vec3 albedo;
uniform float roughness;
uniform float metallic;
uniform float alpha;

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
    vec3 worldPos = (camera.invView * vec4(viewPos, 1)).xyz;
    vec3 viewDir = normalize(camera.position - worldPos);

    // PBR
    vec3 halfVector = normalize(viewDir + dirLight.direction);
    vec3 reflectionVector = reflect(-viewDir, worldNormal);

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

    vec3 ambient = (kD * diffuse + envSpecular);
#else
    vec3 ambient = kD * vec3(0.2) * albedo;
#endif

    // shadows
#ifdef SHADOW_ENABLED
    vec4 fragPosLightSpace = dirLight.transformMatrix * vec4(worldPos, 1.0);
    float shadow = shadowCalculation(fragPosLightSpace, worldNormal);
    
    vec3 lighting = ambient + (1.0 - shadow) * lightOutput;
#else
    vec3 lighting = ambient + lightOutput;
#endif

    fragColor = vec4(lighting, alpha);
}