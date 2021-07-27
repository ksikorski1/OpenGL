#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"

// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;	// id programu
GLuint idProgram2;  // id 2 programu
GLuint idProgram3;
GLuint idProgram4;
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow

int WindowWidth = 800;
int WindowHeight = 400;

float Brightness = 0.0;

#define NUMBER_OF_TRIANGLES 2

double r2()
{
    double random_value = (double)rand()/RAND_MAX*2.0-1.0;
    return random_value;
}
GLfloat random_triangle_array[NUMBER_OF_TRIANGLES*3*2];

void random_triangles()
{
    srand ( time ( NULL));
    printf("%f", random_triangle_array[0]);
    for (int i = 0; i < (NUMBER_OF_TRIANGLES*3*2); i++){
        //korzystanie w ten sposób z globalnej tablicy w c to nie jest najlepszy pomysl
        random_triangle_array[i] = r2();
    }
}
// ------------------------------------
void display_triangles( GLuint idProgram ){
	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );

	glUseProgram( idProgram );
	//wysylamy pod zmienna uniform o nazwie jasnosc zawartosc zmiennej Brightness
	glUniform1f( glGetUniformLocation( idProgram, "Jasnosc" ), Brightness );
	// Generowanie obiektow na ekranie
	glDrawArrays( GL_TRIANGLES, 0, NUMBER_OF_TRIANGLES*3 );

	glUseProgram( 0 );
	glBindVertexArray( 0 );
}

// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT );

	//----------------------------
	glViewport( 0, 0, WindowWidth/2, WindowHeight/2 );
    //krok 2 Potok renderingu
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    display_triangles(idProgram);

	//2
	//-------------------------------

    glViewport( WindowWidth/2, 0, WindowWidth, WindowHeight/2 );
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    display_triangles(idProgram2);

	//3
	//----------------------------------------

    glViewport( 0, WindowHeight/2, WindowWidth/2, WindowHeight - 5 );
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    display_triangles(idProgram3);

	//4
	//------------------------------------

    glViewport( WindowWidth/2, WindowHeight/2, WindowWidth/2, WindowHeight - 5 );
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    display_triangles(idProgram4);

    //-------------

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
    // kolory okienka
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	//tworzy losowa tablice wspolrzednych o liczbie trojkatow rownej NUMBER_OF_TRIANGLES
    random_triangles();


	// 1. Program i shadery
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( idProgram );

	//tak samo dla 2 programu - osobne shadery
	idProgram2 = glCreateProgram();
	glAttachShader(idProgram2, LoadShader(GL_VERTEX_SHADER, "vertex-2.glsl"));
	glAttachShader(idProgram2, LoadShader(GL_FRAGMENT_SHADER, "fragment-2.glsl"));
	LinkAndValidateProgram(idProgram2);


	//3
	idProgram3 = glCreateProgram();
	glAttachShader(idProgram3, LoadShader(GL_VERTEX_SHADER, "vertex-3.glsl"));
	glAttachShader(idProgram3, LoadShader(GL_FRAGMENT_SHADER, "fragment-3.glsl"));
	LinkAndValidateProgram(idProgram3);

	//4
	idProgram4 = glCreateProgram();
	glAttachShader(idProgram4, LoadShader(GL_VERTEX_SHADER, "vertex-4.glsl"));
	glAttachShader(idProgram4, LoadShader(GL_FRAGMENT_SHADER, "fragment-4.glsl"));
	LinkAndValidateProgram(idProgram4);


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );

	//ZMIENIONE ABY UZYWAC LOSOWEJ TABLICY WIERZCHOLKOW
	glBufferData( GL_ARRAY_BUFFER, sizeof( random_triangle_array ), random_triangle_array, GL_STATIC_DRAW );
	//glBufferData( GL_ARRAY_BUFFER, sizeof( random_triangle_array ), random_triangle_array, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	WindowWidth = width;
    WindowHeight = height;
}

// ---------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;
        case 'w':
            Brightness += 0.1;
            break;
        case 's':
            Brightness -= 0.1;
            break;

    }
    printf("Brightness = %f\n", Brightness);

    glutPostRedisplay();
}


// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( 500, 500 );
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
	glDeleteProgram( idProgram2 );
	glDeleteProgram( idProgram3 );
	glDeleteProgram( idProgram4 );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
