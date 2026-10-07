#version 450

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 aColor;
layout(location = 1) in vec3 aBgColor;
layout(location = 2) in mat4 aEffect;

void main(){
    vec2 uv = (gl_FragCoord.xy / vec2(800.0, 600.0)) * 2.0 - 1.0;
    uv.x *= (800.0 / 600.0);

    vec4 transformedUV = aEffect * vec4(uv, 0.0, 1.0);
    vec2 p = transformedUV.xy;

    float d = length(p);
    float pulse = sin(d * 0.8) * 0.5 + 0.5;

    float angle = atan(p.y, p.x);
    float rays = sin(angle * 6.0) * 0.5 + 0.5;

    float intensity = smoothstep(0.2, 0.8, pulse * rays);

    float shift = transformedUV.w * 0.1;

    vec3 patternColor;
    patternColor.r = intensity * (aColor.r + shift);
    patternColor.g = intensity * (aColor.g - shift);
    patternColor.b = intensity * (aColor.b + sin(shift));

    vec3 finColor = mix(aBgColor, patternColor, clamp(intensity + 0.1 / (d + 0.1), 0.0, 1.0));

    fragColor = vec4(finColor, 1.0);
}
