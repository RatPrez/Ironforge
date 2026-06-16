#version 330

in vec3 fragNormal;

uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec3 lightDir = normalize(vec3(0.4, 1.0, 0.3));
    vec3 n = normalize(fragNormal);
    float diff = max(dot(n, lightDir), 0.0);
    float lighting = 0.35 + diff * 0.65;
    finalColor = vec4(colDiffuse.rgb * lighting, colDiffuse.a);
}
