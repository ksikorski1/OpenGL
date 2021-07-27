#version 150

in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

out vec4 outColor;

// Sampler tekstury
uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{

	// kolor fragmentu bedzie polaczeniem koloru z tekstury i
	// koloru z atrybutow wierzcholkow

	//outColor = texture( tex0, inoutUV );// + vec4(inoutColor, 1.0) ;

	//outColor = texture( tex1, inoutUV2 );// + vec4(inoutColor, 1.0) ;
	float mix_amount = ((inoutUV.s + 0.2) - inoutUV.t) * 0.5;
	outColor = mix(texture(tex0, inoutUV), texture(tex1, inoutUV), mix_amount);
}
