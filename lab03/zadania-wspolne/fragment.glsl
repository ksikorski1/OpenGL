#version 330

// Predefiniowane dane wejsciowe
// in vec3 gl_FragCoord;
// in int  gl_PrimitiveID;
in vec3 fragColor;

uniform float Jasnosc;

// Dane wyjsciowe
out vec4 outColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
	outColor += vec4(vec3(Jasnosc), 0.0);
}
