#version 150

in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

out vec4 outColor;

// Sampler tekstury
uniform sampler2D tex0;

void main()
{

	// kolor fragmentu bedzie polaczeniem koloru z tekstury i
	// koloru z atrybutow wierzcholkow

	outColor = texture( tex0, inoutUV );// + vec4(inoutColor, 1.0) ;

}
