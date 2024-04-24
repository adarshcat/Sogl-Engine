#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D hdrImage;

void main(){
    vec4 hdrSample = texture(hdrImage, texCoord);
    if (hdrSample.a < 0.001) discard;

    vec3 color = hdrSample.rgb;

    vec3 tonemapped = color/(color+1.0f);
    float gamma = 2.2;
    FragColor = vec4(pow(tonemapped, vec3(1.0/gamma)), 1.0);
}