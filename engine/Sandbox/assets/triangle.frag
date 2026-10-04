#version 450

layout(location = 0) out vec4 fragColor;

layout(set = 3, binding = 0, std140) uniform MatBuffer{
    vec3 uColor;
    float uTime;
};

void main(){
    vec3 blink = uColor * (sin(2 * uTime) * 0.5 + 0.5);
    fragColor = vec4(blink, 1.0);
}
