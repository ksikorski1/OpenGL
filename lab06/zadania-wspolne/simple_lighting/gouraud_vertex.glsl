#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec3 ourColor;

uniform mat4 Matrix_proj_mv;

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
	gl_Position = Matrix_proj_mv * inPosition;


	// przeslanie obliczonej wartosci swiatla
	// do fragment shadera, ktory go uwzgledni
	ourColor = Calculate_PointLight(vec3(inPosition), inNormal);

}
