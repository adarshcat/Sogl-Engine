#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 albedo;

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = vec3(1.0, 0.0, 0.0);//texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = 1.0f;//texture(texture_specular1, TexCoords).r;
}


// void main(){
//     vec3 lightColor = vec3(1.0);
//     vec3 lightPos = vec3(3.0, 3.0, 3.0);
//     vec3 lightDir = normalize(lightPos - fragPos);
//     float lightStrength = 2.2f;

//     float ambientStrength = 0.2;
//     vec3 ambient = ambientStrength * lightColor;

//     float diff = max(dot(worldNormal, lightDir), 0.0);
//     vec3 diffuse = diff * lightColor * lightStrength;

//     float specularStrength = 0.75;
//     vec3 viewDir = normalize(cameraPos -fragPos);
//     vec3 reflectDir = reflect(-lightDir, worldNormal);

//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     vec3 specular = specularStrength * spec * lightColor;

//     vec3 result = (ambient + diffuse + specular) * albedo;
//     FragColor = result/(result+1.0f);
// }