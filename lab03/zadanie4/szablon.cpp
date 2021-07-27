#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <time.h>

#include "shader_stuff.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "teapot.h"
#include "szescian.h"

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

GLuint idVAO2;
GLuint idVBO_coord2;	// bufora wierzcholkow
GLuint idVBO_color2;
GLuint vBuffer_indices2;

float Brightness = 0.0;

float RotationX = 0.0f;
float RotationY = 0.0f;

int WindowWidth = 800;
int WindowHeight = 800;


// ---------------------------------------
// Wspolrzedne wierzchokow
#define NUMBER_OF_TRIANGLES	12
#define NUM_OF_OBJECTS 5

double r2()
{
    double random_value = (double)rand()/RAND_MAX*2.0;
    return random_value;
}



void fill_array(glm::vec3 *random){
    for (int i = 0; i < NUM_OF_OBJECTS; i++){
        random[i] = glm::vec3(r2(), r2(), r2());
    }
}


glm::vec3 random1[NUM_OF_OBJECTS];
glm::vec3 random2[NUM_OF_OBJECTS];

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

    //METODA SPRAWIA ZE PRZY KAZDYM OBROCIE SKALA TEZ JEST ZMIENIANA

    // ---------------- CZAJNIK
	// Generowanie obiektow na ekranie
	for (int i = 0; i < NUM_OF_OBJECTS; i++){
        matView = glm::mat4(1.0);
        matView = glm::translate(matView, glm::vec3(-8.0+(i*4), 4.0 - i, -10.0));

        matView = glm::rotate(matView, RotationY, glm::vec3(0.0, 1.0, 0.0));
        matView = glm::rotate(matView, RotationX, glm::vec3(1.0, 0.0, 0.0));

        matView = glm::scale(matView, random1[i]);

        glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjection"), 1, GL_FALSE, &matProjection[0][0] );
        glUniformMatrix4fv( glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, &matView[0][0] );

        glDrawElements( GL_TRIANGLES, TEAPOT_INDICES_COUNT * 3, GL_UNSIGNED_INT, NULL );
	}
	glBindVertexArray( 0 );

    //SZESCIAN
    //-------------------------------------------------------------------------------------

    glBindVertexArray( idVAO2 );
	for (int j = 0; j < 5; j++){
        matView = glm::mat4(1.0);
        matView = glm::translate(matView, glm::vec3(-8.0+(j*4), -4.0 - j, -10.0));

        matView = glm::scale(matView, random2[j]);

        matView = glm::rotate(matView, RotationY, glm::vec3(0.0, 1.0, 0.0));
        matView = glm::rotate(matView, RotationX, glm::vec3(1.0, 0.0, 0.0));

        glUniformMatrix4fv( glGetUniformLocation(idProgram, "matProjection"), 1, GL_FALSE, &matProjection[0][0] );
        glUniformMatrix4fv( glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, &matView[0][0] );

        glDrawElements( GL_TRIANGLES, NUMBER_OF_TRIANGLES * 3, GL_UNSIGNED_INT, NULL );
	}



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

	//tworze 2 macierze tutaj, aby przy kazdej zmianie obrazu ich juz nie modyfikowac
	srand ( time ( NULL));
    fill_array(random1);
    fill_array(random2);

	// 1. Program i shadery
	idProgram = glCreateProgram();

	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( idProgram );


	//--------------------------------------------------------------------------------------------
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


	glGenBuffers( 1, &vBuffer_indices );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_indices );
    //lepiej sizeof (typ) * jego_ilosc
    //typ - np. GLfloat albo GLuint
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( teapotIndices ), teapotIndices, GL_STATIC_DRAW );

	glBindVertexArray( 0 );


    //SZESCIAN
    //----------------------------------------------------------------------------------------

    glGenVertexArrays( 1, &idVAO2 );
	glBindVertexArray( idVAO2 );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord2 );

	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord2 );
	glBufferData( GL_ARRAY_BUFFER, sizeof( pozycje_wierzcholkow ), pozycje_wierzcholkow, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );



	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_color2 );

	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color2 );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


	glGenBuffers( 1, &vBuffer_indices2 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vBuffer_indices2 );
    //lepiej sizeof (typ) * jego_ilosc
    //typ - np. GLfloat albo GLuint
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indeksy ), indeksy, GL_STATIC_DRAW );

	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	WindowWidth = width;
	WindowHeight = height;

	// Ustalona rodzaj kamery: perpektywiczna o szerokosci widoku 90*
	matProjection = glm::perspective(glm::radians(90.0f), width/(float)height, 0.01f, 40.0f );


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
	glDeleteVertexArrays( 1, &idVBO_color );
	glDeleteVertexArrays( 1, &vBuffer_indices );

	glDeleteVertexArrays( 1, &idVBO_coord2 );
	glDeleteVertexArrays( 1, &idVAO2 );
	glDeleteVertexArrays( 1, &idVBO_color2 );
	glDeleteVertexArrays( 1, &vBuffer_indices2 );

	return 0;
}
