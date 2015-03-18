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
	const float radius = 1.f;
	float variation = .2f;
	float maxHeight = radius + variation;
	float minHeight = radius - variation / 2;

	float height = length(In.Position);
	height = clamp((height - minHeight) / (maxHeight - minHeight), 0.01f, .99f); // transform height between 0 - 1 to uv tex

    vec3 diffuse = texture(Diffuse, vec2(0, -height)).rgb;
    FragColor = vec4(diffuse, 1.0);
}