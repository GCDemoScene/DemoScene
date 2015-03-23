#version 430 core

#define FRAG_COLOR  0

precision highp int;

uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform float SpecularPower;
uniform float Radius;
uniform vec3 PlanetPosition;

// Write in GL_COLOR_ATTACHMENT0
layout(location = 0) out vec4 Color;
// Write in GL_COLOR_ATTACHMENT1
layout(location = 1) out vec4 Normal;

in block
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 Position;
} In;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	const float radius = Radius;
	float variation = radius / 5.f;
	float maxHeight = radius + variation;
	float minHeight = radius - variation / 2;

	float height = length(In.Position - PlanetPosition);
	height = clamp((height - minHeight) / (maxHeight - minHeight), 0.01f, .99f); // transform height between 0 - 1 to uv tex

    vec3 diffuse = texture(Diffuse, vec2(rand(In.TexCoord), -height)).rgb;
    vec3 specular = texture(Specular, vec2(rand(In.TexCoord), -height)).rgb;

    Normal.rgb = In.Normal;
	Normal.a = SpecularPower;
	Color.rgb = diffuse;
	Color.a = specular.r;
}