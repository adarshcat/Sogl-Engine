#version 330 core

out vec3 FragColor;

in vec2 texCoord;

uniform mat4 invViewMatrix;
uniform vec3 cameraPos;

uniform sampler2D gPositionView;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct DirectionalLight {
    sampler2D shadowMap;
    vec3 color;
    vec3 direction;
    float strength;
    mat4 transformMatrix;
};

uniform DirectionalLight dirLight;

float shadowCalculation(vec4 fragPosLightSpace, vec3 normal){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(dirLight.shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.01 * (1.0 - dot(normal, dirLight.direction)), 0.005);

    // shadow caculation with PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirLight.shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(dirLight.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;   
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main(){
    // Retrieve the values from g buffer
    vec3 worldNormal = texture(gNormal, texCoord).rgb;
    if (length(worldNormal) == 0.0){
        discard;
    }

    vec3 viewPos = texture(gPositionView, texCoord).rgb;
    vec3 worldPos = (invViewMatrix * vec4(viewPos, 1)).xyz;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;

    // phong lighting, start with ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * dirLight.color;

    // calculate diffuse
    vec3 diffuse = max(dot(worldNormal, dirLight.direction), 0.0) * dirLight.color * dirLight.strength;

    // calculate specular
    float specularStrength = 0.75;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-dirLight.direction, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * dirLight.color;

    // calculate shadows
    vec4 fragPosLightSpace = dirLight.transformMatrix * vec4(worldPos, 1.0);
    float shadow = shadowCalculation(fragPosLightSpace, worldNormal);
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;

    // output color with rienhard tonemapping
    FragColor = lighting/(lighting+1.0f);
}