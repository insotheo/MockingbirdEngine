#version 450

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main(){
    fragTexCoord = vTexCoord;
    gl_Position = vec4(vPos, 0.0, 1.0);
}
