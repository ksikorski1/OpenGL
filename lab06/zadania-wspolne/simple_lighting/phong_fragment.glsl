#version 150 core

in vec3 ourPosition;
in vec3 ourNormal;

out vec4 outColor;

// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Specular;
uniform vec3 Light_Position;
uniform vec3 Camera_Position;

// ---------------------------------------------------------------------------
vec3 Calculate_PointLight(vec3 fragPos, vec3 fragNormal)
{
	// ---------------------------
	// Ambient
	vec3 ambientPart = Light_Ambient;


	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = normalize(Light_Position - vec3(fragPos));

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDirection), 0.0);

	vec3 diffusePart = lightCoeff * Light_Diffuse;


	// ------------------
	// Specular
	vec3 viewDir = normalize(Camera_Position - vec3(fragPos));
	vec3  reflectDir = reflect(-lightDirection, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 128.0f);
	vec3  specularPart = specularCoeff * Light_Specular;

	// -----------------
	// Ostateczny
	return (ambientPart + diffusePart + specularPart);
}


void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);


	// Zastosowanie oswietlenia do fragmentu
	vec3 result = Calculate_PointLight(ourPosition, ourNormal) * objectColor;

	outColor = vec4(result, 1.0);

}
