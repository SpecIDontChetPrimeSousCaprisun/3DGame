#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 color;
uniform float alpha;

uniform bool hasTexture;
uniform sampler2D tex;

void main()
{
    vec4 finalColor;

    if (hasTexture)
    {
        finalColor = texture(tex, TexCoord);
    }
    else
    {
        finalColor = vec4(color, alpha);
    }

    FragColor = finalColor;
}
