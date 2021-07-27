#version 330 core
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matModel;

// Dane wyslane do geometry shadera
out vData
{
    vec2 UV;
	vec3 Normal;
} vertex;



void main()
{
	// Uwaga: brak macierzy rzutowania, 
	// ktore zostana uzyte w shaderze geometrii
	
	gl_Position = matModel * inPosition;
	

	vertex.UV = inUV;
	vertex.Normal = inNormal;

}
