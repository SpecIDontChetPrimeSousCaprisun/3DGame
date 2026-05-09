#version 330 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;              // ADD THIS
in vec4 FragPosLightSpace;

out vec4 FragColor;

#define MAX_LIGHTS 8

uniform int lightCount;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 color;
uniform sampler2D texture1;
uniform sampler2D shadowMap;
uniform bool useTexture;

float ShadowCalculation(vec4 fragPosLightSpace)
{
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

  projCoords = projCoords * 0.5 + 0.5;

  if (projCoords.z > 1.0)
      return 0.0;

  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;

  float bias = 0.005;

  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

  return shadow;
}

void main()
{
  float shadow = ShadowCalculation(FragPosLightSpace);

  vec3 result = vec3(0.0);

  for (int i = 0; i < lightCount; i++) {
    vec3 L = normalize(lightPos[i] - FragPos);
    float diff = max(dot(Normal, L), 0.0);

    result += diff * lightColor[i];
  }

  vec3 baseColor;

  if (useTexture)
      baseColor = texture(texture1, TexCoord).rgb;
  else
      baseColor = color;

  vec3 lighting = baseColor * result;

  // apply shadow (0 = lit, 1 = fully shadowed)
  lighting *= (1.0 - shadow);

  FragColor = vec4(lighting, 1.0);
}
