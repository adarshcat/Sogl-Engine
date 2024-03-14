#version 330 core

out vec3 FragColor;

in vec2 texCoord;

uniform mat4 invViewMatrix;
uniform vec3 cameraPos;

uniform mat4 dirLightMatrix;

uniform sampler2D gPositionView;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D shadowMap;

float shadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.5 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;   
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main(){
    vec3 worldNormal = texture(gNormal, texCoord).rgb;
    if (length(worldNormal) == 0.0){
        discard;
    }

    vec3 viewPos = texture(gPositionView, texCoord).rgb;
    vec3 worldPos = (invViewMatrix * vec4(viewPos, 1)).xyz;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;

    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(vec3(2.0f, 4.0f, -1.0f));
    float lightStrength = 2.2f;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightStrength;

    float specularStrength = 0.75;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 fragPosLightSpace = dirLightMatrix * vec4(worldPos, 1.0);
    float shadow = shadowCalculation(fragPosLightSpace, lightDir, worldNormal);

    

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;    

    FragColor = lighting/(lighting+1.0f);
}