#version 410 core

#define POSITION    0
#define NORMAL      1
#define TEXCOORD    2

precision highp float;
precision highp int;

uniform mat4 MVP;
uniform int Time;
uniform ivec2 Discretization;

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 TexCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

out block
{
    vec2 TexCoord; 
    vec3 Normal;
    vec3 Position;
} Out;

bool isEdge()
{
	int nbVerticesX = Discretization[0] + 1;
	int nbVerticesY = Discretization[1] + 1;
	int verticesPerFace = nbVerticesX * nbVerticesY; //width and height of discretization represent quad and not vertices.
	// int faceFamily = gl_VertexID/verticesPerFace;
	
	int relativePosition = gl_VertexID % verticesPerFace;
	int x = relativePosition % nbVerticesX;
	int y = relativePosition / nbVerticesX;

	if((x > 0 && x < nbVerticesX - 1) && (y > 0 && y < nbVerticesY - 1))
		return false;
	return true;
}

void main()
{
	vec3 variation = vec3(0);
	vec3 normal = Normal;
	if(!isEdge()) // Keep edges stuck
	{
	    variation = Normal * ((cos(Time * 0.0007) + 1) / 100) * (cos((gl_VertexID + Time * 0.02) * 0.01)); // Where magics happens (variantions of edges)
	}
	vec3 pos = Position;

	pos.x = Position.x * cos(Time* 0.00007) - Position.z * sin(Time* 0.00007);
	pos.y = Position.y;
	pos.z = Position.x * sin(Time* 0.00007) + Position.z * cos(Time* 0.00007);

	pos += variation;

	normal.x = Normal.x * cos(Time* 0.00007) - Normal.z * sin(Time* 0.00007);
	normal.y = Normal.y;
	normal.z = Normal.x * sin(Time* 0.00007) + Normal.z * cos(Time* 0.00007);

    Out.TexCoord = TexCoord;
    Out.Normal = Normal;
    Out.Position = pos;
    
    gl_Position = MVP * vec4(pos, 1.0);
}