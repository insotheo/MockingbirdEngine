#version 450

layout(location = 0) out vec4 fragColor;

layout(set = 3, binding = 0, std140) uniform MatBuffer{
    vec3 uColor;
    vec3 uBgColor;
    mat4 uEffect;
};

void main(){
    vec2 uv = (gl_FragCoord.xy / vec2(800.0, 600.0)) * 2.0 - 1.0;
    uv.x *= (800.0 / 600.0);

    vec4 transformedUV = uEffect * vec4(uv, 0.0, 1.0);
    vec2 p = transformedUV.xy;

    float d = length(p);
    float pulse = sin(d * 0.8) * 0.5 + 0.5;

    float angle = atan(p.y, p.x);
    float rays = sin(angle * 6.0) * 0.5 + 0.5;

    float intensity = smoothstep(0.2, 0.8, pulse * rays);

    float shift = transformedUV.w * 0.1;

    vec3 patternColor;
    patternColor.r = intensity * (uColor.r + shift);
    patternColor.g = intensity * (uColor.g - shift);
    patternColor.b = intensity * (uColor.b + sin(shift));

    vec3 finColor = mix(uBgColor, patternColor, clamp(intensity + 0.1 / (d + 0.1), 0.0, 1.0));

    fragColor = vec4(finColor, 1.0);
}
