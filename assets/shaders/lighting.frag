#version 330 core

out vec3 FragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main(){
    FragColor = texture(gPosition, texCoord).rgb;
}