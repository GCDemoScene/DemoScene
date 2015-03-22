#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2

layout(location = POSITION) in vec3 Position;

uniform mat4 ObjectToLightScreen;

void main()
{	
	vec3 pos = Position;

	gl_Position = ObjectToLightScreen * vec4(pos, 1.0);
}