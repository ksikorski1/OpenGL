#version 330 core

layout( location = 0 ) in vec4 inPosition;
uniform mat4 Matrix_proj_view;
out vec4 fragPos;

void main()
{
	gl_Position = Matrix_proj_view * inPosition;
	fragPos = inPosition;
}
