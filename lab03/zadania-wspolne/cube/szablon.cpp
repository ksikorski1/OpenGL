#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.h"

// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord; // bufora wierzcholkow
GLuint idVBO_color;
int window_width = 500;
int window_height = 500;
float rotation_x = 0.1;
float rotation_y = 0.1;
float Brightness = 0.0;

// rodzaj kamery
glm::mat4 matProjection;
// polozenie kamery
glm::mat4 matView;


// ---------------------------------------
// Wspolrzedne wierzchokow

#define NUMBER_OF_TRIANGLES	12

// Wspolrzedne wierzchokow
GLfloat triangles_coord[6*6*3] =
{
	// Top face
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,

	// Bottom face
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,

	// Front face
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,

	// Back face
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,

	// Left face
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,

	// Right face
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f
};


// Kolory wierzcholkow
GLfloat triangles_color[6*6*3] =
{
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	1.0f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,
	1.0f, 0.5f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f,
	0.5f, 0.0f, 0.5f
};

// ---------------------------------------
void DisplayScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	matView = glm::mat4(1.0f);
	matView = glm::translate(matView, glm::vec3(0.0, 0.0, -2.0));
	matView = glm::rotate(matView, rotation_y, glm::vec3(0.0, 1.0, 0.0));
	matView = glm::rotate(matView, rotation_x, glm::vec3(1.0, 0.0, 0.0));

	// viewport 1
	glViewport(0, 0, window_width, window_height);

	// Wlaczenie VAO i programu
	glBindVertexArray(idVAO);
	glUseProgram(idProgram);


	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProjection"), 1, GL_FALSE, &matProjection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, &matView[0][0]);

	//wysylamy pod zmienna uniform o nazwie jasnosc zawartosc zmiennej Brightness
	glUniform1f(glGetUniformLocation(idProgram, "Jasnosc"), Brightness);

	// Generowanie obiektow na ekranie
	glDrawArrays(GL_TRIANGLES, 0, NUMBER_OF_TRIANGLES * 3);

	// Wylaczanie
	glUseProgram(0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	glEnable(GL_DEPTH_TEST);

	// kolory okienka
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// 1. Program i shadery
	idProgram = glCreateProgram();

	glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram(idProgram);

	// 2. Vertex arrays
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_coord), triangles_coord, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Bufor na kolory
	glGenBuffers(1, &idVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_color), triangles_color, GL_STATIC_DRAW);

	// Tablica kolorow - 1 parametr to id, drugi to skladowa (3 bo rgb)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

// ---------------------------------------
void Reshape(int width, int height)
{
	//glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;

	// kamera
	matProjection = glm::perspective(90.0f, width/(float)height, 1.1f, 5.0f);
}

// ---------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC key
		exit(0);
		break;
	case 'w':
		Brightness += 0.1;
		break;
	case 's':
		Brightness -= 0.1;
		break;
	case 'a':
		rotation_x += 0.1;
		break;
	case 'd':
		rotation_y += 0.1;
		break;
	}
	printf("Brightness = %f\n", Brightness);

	glutPostRedisplay();
}

// ---------------------------------------------------
int main(int argc, char *argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("OpenGL!");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);

	// Keyboard
	glutKeyboardFunc(Keyboard);

	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if (!GLEW_VERSION_3_2)
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}

	Initialize();
	glutMainLoop();

	// Cleaning
	glDeleteProgram(idProgram);
	glDeleteVertexArrays(1, &idVBO_coord);
	glDeleteVertexArrays(1, &idVAO);

	return 0;
}
