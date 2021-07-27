#version 150

in vec4 inoutPos;
out vec4 outColor;


void main()
{

	// odleglosc od poczatku ukladu wspolrzednych
    float odl = sqrt(inoutPos.x*inoutPos.x + inoutPos.y*inoutPos.y + inoutPos.z*inoutPos.z);

    // szalone kolory
    vec3 crazyColor = vec3(
		sin(odl*3f)/2f + 0.5f,
		sin(inoutPos.y)/2f + 0.5f,
		sin(gl_PrimitiveID/100.0)/5f + 0.5f
	);

	vec3 myColor = crazyColor;

	outColor = vec4(myColor, 1.0);
}
