#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform float bloomStrength = 0.05;

uniform sampler2D hdrImage;
uniform sampler2D bloomImage;

void main(){
    vec3 hdrSample = texture(hdrImage, texCoord).rgb;
    vec3 bloomSample = texture(bloomImage, texCoord).rgb;

    vec3 color = hdrSample + bloomSample * bloomStrength;

    vec3 tonemapped = color/(color+1.0f);
    float gamma = 2.2;
    FragColor = vec4(pow(tonemapped, vec3(1.0/gamma)), 1.0);
}