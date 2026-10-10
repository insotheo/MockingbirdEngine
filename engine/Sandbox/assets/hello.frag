#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform sampler2D surfaceTexture;

layout(set = 3, binding = 0, std140) uniform MatBuffer {
    float uTime;
};

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(){
    vec2 uv = fragTexCoord;

    float jitter = sin(uv.y * 50.0 + uTime * 15.0) * sin(uv.y * 130.0 + uTime * 23.0);
    if(rand(vec2(uTime, floor(uv.y * 40))) > 0.95){
        uv.x += jitter * 0.015;
    }

    float chromaShift = 0.006 + 0.003 * sin(uTime * 5.0);
    float r = texture(surfaceTexture, vec2(uv.x + chromaShift, uv.y)).r;
    float g = texture(surfaceTexture, uv).g;
    float b = texture(surfaceTexture, vec2(uv.x - chromaShift, uv.y)).b;
    vec3 color = vec3(r, g, b);

    float noise = rand(uv * uTime) * 0.12;
    color += vec3(noise);

    color = mix(color, vec3(dot(color, vec3(0.299, 0.587, 0.114))), 0.15);
    color.rgb *= vec3(0.95, 1.0, 0.9);

    fragColor = vec4(color, 1.0);
}
