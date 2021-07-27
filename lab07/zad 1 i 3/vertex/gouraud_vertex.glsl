#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec4 inNormal;
layout( location = 2 ) in vec2 inUV;

out vec4 ourColor;
out vec2 inoutUV;

// Oswietlenie
struct SLight
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Position;
    float Attenuation;
};

uniform struct SLight myLight;

uniform mat4 matProjViewModel;
uniform mat4 matModel;
uniform mat3 matNormal;
uniform vec3 Camera_Position;

// -----------------------
// Materialy
struct SMaterial
{
	vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Shininess;
};

uniform struct SMaterial myMaterial;


// ---------------------------------------------------------------------------
vec4 Calculate_PointLight(vec3 fragPos, vec3 fragNormal, struct SLight myLight, struct SMaterial myMaterial)
{

	// Distance light
	float Light_dist = length(vec3(myLight.Position) - fragPos);
	float Attenuation = 1.0 / (1.0 + myLight.Attenuation * (Light_dist * Light_dist));

	// ---------------------------
	// Ambient
	vec4 ambientPart = myLight.Ambient * myMaterial.Ambient * Attenuation;


	// ---------------------------
	// Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = normalize(vec3(myLight.Position) - fragPos);

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(fragNormal, lightDirection), 0.0);

	vec4 diffusePart = lightCoeff * myLight.Diffuse * myMaterial.Diffuse * Attenuation;


	// ------------------
	// Specular
	vec3 viewDir = normalize(Camera_Position - fragPos);
	vec3  reflectDir = reflect(-lightDirection, fragNormal);
	// obliczanie wspolczynnika specular z parametrem shininess
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), myMaterial.Shininess);
	vec4  specularPart = specularCoeff * myLight.Specular * myMaterial.Specular * Attenuation;

	// -----------------
	// Ostateczny
	return (ambientPart + diffusePart + specularPart);
}


void main()
{
	gl_Position = matProjViewModel * inPosition;
    inoutUV = inUV;

    vec3 myPosition = vec3(matModel * inPosition);
	vec3 myNormal = matNormal * vec3(inNormal);

	ourColor = Calculate_PointLight(myPosition, myNormal, myLight, myMaterial);

}
