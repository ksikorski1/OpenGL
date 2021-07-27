#version 150 core

// kolor wejsciowy uwzglednia oswietlenie
// w modelu Gourauda obliczone
// w vertex shaderze
in vec3 ourColor;

out vec4 outColor;


void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	vec3 result = ourColor * objectColor;

	outColor = vec4(result, 1.0);

}
