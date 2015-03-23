#version 410 core

#define POSITION    0
#define NORMAL      1
#define TEXCOORD    2

precision highp float;
precision highp int;

uniform mat4 MVP;
uniform int Time;
uniform ivec2 Discretization;
uniform vec3 PlanetPosition;

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
    vec3 PlanetPosition;
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
	float coeff = (abs(PlanetPosition.x) * 0.0001) + 0.00007 ;

	pos.x = Position.x * cos(Time* coeff) - Position.z * sin(Time*coeff);
	pos.y = Position.y;
	pos.z = Position.x * sin(Time* coeff) + Position.z * cos(Time*coeff);

	pos += variation;

	Out.PlanetPosition.x = PlanetPosition.x * cos(Time* coeff) - PlanetPosition.z * sin(Time*coeff);
	Out.PlanetPosition.y = PlanetPosition.y;
	Out.PlanetPosition.z = PlanetPosition.x * sin(Time* coeff) + PlanetPosition.z * cos(Time* coeff);

	normal.x = Normal.x * cos(Time* coeff) - Normal.z * sin(Time* coeff);
	normal.y = Normal.y;
	normal.z = Normal.x * sin(Time* coeff) + Normal.z * cos(Time* coeff);

    Out.TexCoord = TexCoord;
    Out.Normal = Normal;
    Out.Position = pos;
    
    gl_Position = MVP * vec4(pos, 1.0);
}