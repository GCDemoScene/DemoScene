#version 410 core

#define POSITION    0
#define NORMAL      1
#define TEXCOORD    2
#define FRAG_COLOR  0

precision highp int;

uniform sampler2D Diffuse;

layout(location = FRAG_COLOR, index = 0) out vec4 FragColor;

in block
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 Position;
} In;

void main()
{
    FragColor = vec4(In.Position, 1.0);
}