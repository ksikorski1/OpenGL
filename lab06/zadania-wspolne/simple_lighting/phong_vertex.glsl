#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec3 ourPosition;
out vec3 ourNormal;

uniform mat4 Matrix_proj_mv;

void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

	// W Modelu Phonga, oswietlenie obiektu obliczane
	// jest na poziomie fragmentu, a nie wierzcholka
	// jak w modelu Gourauda. Wysylamy wiec wszystkie
	// dane wierzcholka jak pozycje i wektor normalny
	// do shadera fragmentow

	ourPosition = vec3(inPosition);
	ourNormal = inNormal;

}
