#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 color;

void main()
{
    float alpha = texture(tex, TexCoord).r;

    FragColor = vec4(color, alpha);
}
