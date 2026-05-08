#version 330 core

in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 color;
uniform sampler2D texture1;
uniform bool useTexture;

void main() {
  float diff = max(dot(normalize(Normal), normalize(lightDir)), 0.1);

  vec3 baseColor;

  if (useTexture)
      baseColor = texture(texture1, TexCoord).rgb;
  else
      baseColor = color;

  FragColor = vec4(baseColor * diff, 1.0);
}
