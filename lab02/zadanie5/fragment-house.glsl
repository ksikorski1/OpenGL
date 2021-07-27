#version 330

// Predefiniowane dane wejsciowe
// in vec3 gl_FragCoord;
// in int  gl_PrimitiveID;

uniform float Jasnosc;

// Dane wyjsciowe
out vec4 outColor;

void main()
{
	outColor = vec4(0.0, 0.0, 0.0, 1.0);

	//jest 9 trojkatow
	//kazdy ma inne PrimitiveId
	//wiec mozna (recznie) kazdemu (od 0 do 8) trojkatowi nadac inny kolor
	switch(gl_PrimitiveID) {
		case 0:
			outColor = vec4(0.3, 0.0, 0.0, 1.0);
			break;
        case 1:
            outColor = vec4(0.0, 0.3, 0.0, 1.0);
            break;
        case 2:
            outColor = vec4(0.0, 0.0, 0.3, 1.0);
            break;
        case 3:
            outColor = vec4(0.3, 0.3, 0.0, 1.0);
            break;
        case 4:
            outColor = vec4(0.0, 0.3, 0.3, 1.0);
            break;
        case 5:
            outColor = vec4(0.3, 0.3, 0.3, 1.0);
            break;
        case 6:
            outColor = vec4(0.5, 0.2, 0.0, 1.0);
            break;
        case 7:
            outColor = vec4(0.5, 0.0, 0.5, 1.0);
            break;
        case 8:
            outColor = vec4(0.5, 0.8, 0.4, 1.0);
            break;
	}
	outColor += vec4( vec3(Jasnosc), 0.0 );
}
