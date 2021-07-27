// Vertex shader
#version 330

// Dane pobrane z VAO
in vec4 inPosition;

// Predefiniowane dane wejsciowe
// in int gl_VertexID;
// in int gl_InstanceID;

// Dane wyjsciowe (kolejny etap potoku)
// out vec4 gl_Position;

void main()
{
	// zmienna typu vec4
	vec4 finalPosition = vec4(inPosition.y, (inPosition.x * -1), inPosition.z, 1.0);

	// finalna pozycja wierzcholka
	gl_Position = finalPosition;
}
