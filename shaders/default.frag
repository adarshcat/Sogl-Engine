#version 330 core

out vec3 FragColor;

in vec3 fragPos;
in vec3 worldNormal;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 cameraPos;
uniform vec3 albedo;

void main(){
    vec3 lightColor = vec3(1.0);
    vec3 lightPos = vec3(2.0, 3.0, -2.0);
    vec3 lightDir = normalize(lightPos - fragPos);
    float lightStrength = 1.2f;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightStrength;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos -fragPos);
    vec3 reflectDir = reflect(-lightDir, worldNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * albedo;
    FragColor = result;
}