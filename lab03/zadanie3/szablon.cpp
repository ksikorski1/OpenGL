#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "teapot.h"

// Macierz odpowiedzialna za rodzaj widoku (widok perspektywiczny/ortogonalny i jego parametry)
glm::mat4 matProjection;

// Macierz odpowiedzialna za polozenie kamery
glm::mat4 matView;


// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow
GLuint idVBO_color;
GLuint vBuffer_indices;

float Brightness = 0.0;

float RotationX = 0.0f;
float RotationY = 0.0f;

int WindowWidth = 800;
int WindowHeight = 800;


// ---------------------------------------
void DisplayScene()
{


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Krok 1 tworzenie pierwszego viewportu
	glViewport( 0, 0, WindowWidth, WindowHeight );

	// Krok 2 Potok renderingu
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );
	glUseProgram( idProgram );


	// Majac dwie macierze
	// matProjection i matView, trzeba je przekazac do vertex shadera jako zmienne typu uniform
	// Polozenie kamery
	matView = glm::mat4(1.0);
	matView = glm::translate(matView, glm::vec3(0.0, 0.0, -5.0));
	matView = glm::rotate(matView, RotationY, glm::vec3(0.0, 1.0, 0.0));
	matView = glm::rotate(matView, RotationX, glm::vec3(1.0, 0.0, 0.0));

	glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjection"), 1, GL_FALSE, &matProjection[0][0] );
	glUniformMatrix4fv( glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, &matView[0][0] );

	// Generowanie obiektow na ekranie
	//glDrawArrays( GL_TRIANGLES, 0, NUMBER_OF_TRIANGLES*3 );
	glDrawElements( GL_TRIANGLES, TEAPOT_INDICES_COUNT * 3, GL_UNSIGNED_INT, NULL );

	// Wylaczanie
	glUseProgram( 0 );
	glBindVertexArray( 0 );

	glutSwapBuffers();
	printf("DisplayScene()\n");
}

// ---------------------------------------
void Initialize()
{
	glEnable(GL_DEPTH_TEST);

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );


	// 1. Program i shadery
	idProgram = glCreateProgram();

	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( idProgram );


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );

	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
	glBufferData( GL_ARRAY_BUFFER, sizeof( teapotPosition ), teapotPosition, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );



	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_color );

	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color );
	glBufferData( GL_ARRAY_BUFFER, sizeof( teapotNormal ), teapotNormal, GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

	// BUFOR: Indeksy wierzcholkow

    glGenBuffers( 1, &vBuffer_indices );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_indices );
    //lepiej sizeof (typ) * jego_ilosc
    //typ - np. GLfloat albo GLuint
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( teapotIndices ), teapotIndices, GL_STATIC_DRAW );



	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	WindowWidth = width;
	WindowHeight = height;

	// Ustalona rodzaj kamery: perpektywiczna o szerokosci widoku 90*
	matProjection = glm::perspective(glm::radians(90.0f), width/(float)height, 0.01f, 10.0f );
}

// ---------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

		case 'a':
			RotationY += 0.1;
			break;

		case 'd':
			RotationY -= 0.1;
			break;

		case 'w':
			RotationX += 0.1;
			break;

		case 's':
			RotationX -= 0.1;
			break;

    }
    printf("Rotation %f, %f \n", RotationX, RotationY);

    glutPostRedisplay();
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( WindowWidth, WindowHeight );
	glutCreateWindow( "OpenGL!" );
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );

	// Keyboard
	glutKeyboardFunc( Keyboard );


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( GLEW_OK != err )
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if( !GLEW_VERSION_3_2 )
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();


	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
