#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform float near;
uniform float far;

const float blurCutoff = 0.028;

uniform sampler2D depthBuffer;
uniform sampler2D ssaoInput;

float getLinearDepth(float depth){
    float ndc = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));

    return linearDepth;
}

void main(){
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    int counter = 0;

    float depthSampleCurrent = texture(depthBuffer, texCoord).r;
    float depthLinearCurrent = getLinearDepth(depthSampleCurrent);

    for (int x = -2; x < 2; ++x){
        for (int y = -2; y < 2; ++y){
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float depthSample = texture(depthBuffer, texCoord + offset).r;
            float depthLinear = getLinearDepth(depthSample);
            
            if (abs(depthLinear - depthLinearCurrent) < blurCutoff){
                result += texture(ssaoInput, texCoord + offset).r;
                counter++;
            }
        }
    }
    
    FragColor = vec4(result / counter, 0.0, 0.0, 1.0);
}