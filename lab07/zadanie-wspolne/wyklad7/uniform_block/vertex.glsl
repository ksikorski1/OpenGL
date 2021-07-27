#version 330

// Odebranie danych z tablicy wierzcholkow (VAO)
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inColor;

// Struktura do przekazania danych
// do kolejnego etapu renderingu

out VS_OUT
{
	vec4 position;
	vec4 color;
	vec4 normal;
	vec2 uv;

} vs_out;



void main()
{

	vs_out.position = inPosition;
	vs_out.color = inColor;
	//vs_out.normal = ;
	//vs_out.uv = ;

	gl_Position = inPosition;
}
