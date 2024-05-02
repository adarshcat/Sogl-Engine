#version 330 core

layout (location = 0) out vec3 downsample;

const float invGamma = 1.0 / 2.2;

uniform sampler2D srcTexture;
uniform vec2 srcResolution;
uniform int mipLevel;

in vec2 texCoord;

vec3 PowVec3(vec3 v, float p){
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

vec3 ToSRGB(vec3 v) { return PowVec3(v, invGamma); }

float RGBToLuminance(vec3 col){
    return dot(col, vec3(0.2126f, 0.7152f, 0.0722f));
}

float KarisAverage(vec3 col){
    float luma = RGBToLuminance(ToSRGB(col)) * 0.25f;
    return 1.0f / (1.0f + luma);
}


void main(){
    vec2 srcTexelSize = 1.0 / srcResolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    vec3 a = texture(srcTexture, vec2(texCoord.x - 2*x, texCoord.y + 2*y)).rgb;
    vec3 b = texture(srcTexture, vec2(texCoord.x,       texCoord.y + 2*y)).rgb;
    vec3 c = texture(srcTexture, vec2(texCoord.x + 2*x, texCoord.y + 2*y)).rgb;

    vec3 d = texture(srcTexture, vec2(texCoord.x - 2*x, texCoord.y)).rgb;
    vec3 e = texture(srcTexture, vec2(texCoord.x,       texCoord.y)).rgb;
    vec3 f = texture(srcTexture, vec2(texCoord.x + 2*x, texCoord.y)).rgb;

    vec3 g = texture(srcTexture, vec2(texCoord.x - 2*x, texCoord.y - 2*y)).rgb;
    vec3 h = texture(srcTexture, vec2(texCoord.x,       texCoord.y - 2*y)).rgb;
    vec3 i = texture(srcTexture, vec2(texCoord.x + 2*x, texCoord.y - 2*y)).rgb;

    vec3 j = texture(srcTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    switch (mipLevel){
        case 0:
        vec3 groups[5];

        groups[0] = (a+b+d+e) * (0.125f/4.0f);
        groups[1] = (b+c+e+f) * (0.125f/4.0f);
        groups[2] = (d+e+g+h) * (0.125f/4.0f);
        groups[3] = (e+f+h+i) * (0.125f/4.0f);
        groups[4] = (j+k+l+m) * (0.5f/4.0f);
        groups[0] *= KarisAverage(groups[0]);
        groups[1] *= KarisAverage(groups[1]);
        groups[2] *= KarisAverage(groups[2]);
        groups[3] *= KarisAverage(groups[3]);
        groups[4] *= KarisAverage(groups[4]);
        downsample = groups[0]+groups[1]+groups[2]+groups[3]+groups[4];
        break;

        default:
        downsample = e*0.125;
        downsample += (a+c+g+i)*0.03125;
        downsample += (b+d+f+h)*0.0625;
        downsample += (j+k+l+m)*0.125;
        break;
    }

    downsample = max(downsample, 0.0001f);
}
