#version 330 core

// Standardowy fragment shader
// Tutaj ma znalezc sie wszystko to co potrzebne
// jest do wygenerowania obrazu, ktory bedzie 
// przechowany w Environment Map

// Dane odebrane z shadera geometrii

in fData
{
	vec2 UV;
	vec3 Pos;
	vec3 Normal;

} frag;


out vec4 outColor;

void main()
{

	// Ewentualne tekstury 
	// Ewentualne oswietlenie 

	//vec4 myColor = texture( tex0, fragUV );

	outColor = vec4(0.0, 1.0, 0.0, 1.0);
	
}
