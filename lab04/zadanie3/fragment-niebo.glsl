#version 150

in vec4 inoutPos;
out vec4 outColor;


void main()
{

	// odleglosc od poczatku ukladu wspolrzednych
    float odl = sqrt(inoutPos.x*inoutPos.x + inoutPos.y*inoutPos.y + inoutPos.z*inoutPos.z);

    // szalone kolory


	outColor = vec4(0.0, 0.1, 0.9, 1.0);
}
