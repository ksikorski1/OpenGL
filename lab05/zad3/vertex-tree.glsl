#version 330

// Wspolrzedne wierzcholkow
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;


// Macierz rzutowania
uniform mat4 matProjView;

out vec4 inoutPos;
out vec3 inoutColor;
out vec2 inoutUV;

void main()
{
	gl_Position = matProjView * inPosition;
	inoutPos = inPosition;
	inoutColor = inColor;
	inoutUV = inUV;
}
