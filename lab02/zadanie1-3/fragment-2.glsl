// Fragment shader
#version 330


// Predefiniowane dane wejsciowe
//in  vec4 gl_FragCoord;
//in  int  gl_PrimitiveID;


// Dane wyjsciowe
out vec4 outColor;

in vec3 fragColor;


void main()
{
    //zielony trojkat, nie reagujacy na jasnosc do zadania 3
	outColor = vec4(0.0, 1.0, 0.0, 1.0);

}
