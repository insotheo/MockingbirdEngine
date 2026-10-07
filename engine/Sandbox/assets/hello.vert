#version 450

layout(location = 0) in vec2 vPos;

layout(set = 1, binding = 0, std140) uniform MatBuffer {
    float uTime;
    vec3 uColor;
    vec3 uBgColor;
    mat4 uEffect;
};

layout(location = 0) out vec3 aColor;
layout(location = 1) out vec3 aBgColor;
layout(location = 2) out mat4 aEffect;

void main(){
    aColor = uColor;
    aBgColor = uBgColor;
    aEffect = uEffect;

    gl_Position = vec4(vPos.x + sin(uTime * 5.0), vPos.y, 0.0, 1.0);
}
