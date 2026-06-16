#version 330

in vec3 fragNormal;
in vec3 fragWorldPos;

uniform vec4 colDiffuse;
uniform float tileSize;

out vec4 finalColor;

void main()
{
    vec3 lightDir = normalize(vec3(0.4, 1.0, 0.3));
    vec3 n = normalize(fragNormal);
    float diff = max(dot(n, lightDir), 0.0);
    float lighting = 0.35 + diff * 0.65;

    ivec2 tile = ivec2(floor(fragWorldPos.xz / tileSize));
    float checker = mod(float(tile.x + tile.y), 2.0);
    vec3 baseColor = mix(colDiffuse.rgb, colDiffuse.rgb * 0.8, checker);

    finalColor = vec4(baseColor * lighting, colDiffuse.a);
}
