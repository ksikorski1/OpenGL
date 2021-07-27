// Fragment shader
#version 330


// Predefiniowane dane wejsciowe
//in  vec4 gl_FragCoord;
//in  int  gl_PrimitiveID;


// Dane wyjsciowe
out vec4 ostKolor;

in vec3 fragColor;

void main()
{

	//ostKolor = vec4(0.0, 1.0, 0.0, 1.0);
	//ostKolor.r = (gl_FragCoord.x/600.0f)*2;

	//ostKolor.b = (gl_PrimitiveID%2);
	ostKolor = vec4(fragColor, 1.0);

}
