#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <stdio.h>

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutCreateWindow("GLUT");
	// Create OpenGL Context
	glewInit();

	// OpenGL Version
	printf("OpenGL version (%s): \n", glGetString(GL_VERSION));

	// FreeGLUT Version
	GLint glutVer = (glutGet(GLUT_VERSION));
	printf("GLUT version : %d = %d.%d.%d \n", glutVer, glutVer/10000, (glutVer%1000)/100, glutVer%10);

	// OpenGL Extensions:

	int NumberOfExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);

	printf("\nGL_NUM_EXTENSIONS = %d\n", GL_NUM_EXTENSIONS);

	PFNGLGETSTRINGIPROC glGetStringi = 0;
	glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");

	for (GLint i=0; i<NumberOfExtensions; i++)
	{
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
		printf("Ext %d: %s\n", i, extension);
	}

}
