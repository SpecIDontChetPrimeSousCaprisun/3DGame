#version 330 core

in vec3 Normal;
out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 color;

void main()
{
    float diff = max(dot(normalize(Normal), normalize(lightDir)), 0.0);
    vec3 color = color * diff;

    FragColor = vec4(color, 1.0);
}
