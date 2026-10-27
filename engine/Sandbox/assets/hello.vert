#version 450

layout(set = 1, binding = 0, std140) uniform ME_FrameBuffer {
    mat4 uViewProjection;
} Engine;

layout(set = 1, binding = 1, std140) uniform ME_ObjectBuffer {
    mat4 uModel;
} Object;


layout(location = 0) in vec2 vPos;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main(){
    fragTexCoord = vTexCoord;
    gl_Position = Engine.uViewProjection * Object.uModel * vec4(vPos, 0.0, 1.0);
}
