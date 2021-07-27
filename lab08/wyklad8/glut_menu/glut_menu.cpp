#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow
GLuint idVBO_color;

float Brightness = 0.0;


// ---------------------------------------
// Wspolrzedne wierzchokow
#define NUMBER_OF_TRIANGLES	3
GLfloat triangles_coord[3*3*2] =
{
	-0.8f, -0.8f,
	 0.0f, -0.8f,
	-0.4f,  0.0f,

	 0.0f,  0.0f,
	 0.8f,  0.0f,
	 0.4f,  0.8f,

	 -0.6f, 0.2f,
	 -0.2f, 0.2f,
	 -0.4f, 0.6f,
};

// ---------------------------------------
// Kolory wierzcholkow
GLfloat triangles_color[3*3*3] =
{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f,

	1.f, 1.f, 0.f,
	0.f, 1.f, 1.f,
	1.f, 0.f, 1.f,

	1.f, 1.f, 1.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 1.f
};

// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT );
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);


	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );
	glUseProgram( idProgram );

	// Wysylamy pod zmienna uniform o nazwie "Jasnosc"
	// zawartosc zmiennej Brightness
	glUniform1f( glGetUniformLocation(idProgram, "Jasnosc"), Brightness );

	// Generowanie obiektow na ekranie
	glDrawArrays( GL_TRIANGLES, 0, NUMBER_OF_TRIANGLES*3 );


	// Wylaczanie
	glUseProgram( 0 );
	glBindVertexArray( 0 );



	glutSwapBuffers();
	printf("DisplayScene()\n");
}

// ---------------------------------------
void Initialize()
{

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
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_coord ), triangles_coord, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );



	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_color );

	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color );
	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );


	glBindVertexArray( 0 );

}

// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );

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
void Menu( int value )
{
    switch( value )
    {
	case 1:
		printf("Opcja nr jeden\n")	;
		break;

        // GL_TEXTURE_MAG
    }

    printf("Wywolano funkcje Menu(%d)\n", value);
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

	// Utworzenie podmenu
	int podmenuA = glutCreateMenu( Menu );
    glutAddMenuEntry( "Pozycja 1", 1 );
    glutAddMenuEntry( "Pozycja 2", 2 );

	// Utworzenie podmenu
	int podmenuB = glutCreateMenu( Menu );
    glutAddMenuEntry( "Pozycja 3", 3 );
    glutAddMenuEntry( "Pozycja 4", 4 );

	// Utworzenie glownego menu kontekstowego
    glutCreateMenu( Menu );

    glutAddMenuEntry( "Pozycja 5", 5 );
    glutAddSubMenu( "Podmenu A", podmenuA );
    glutAddSubMenu( "Podmenu B", podmenuB );

	// Przypisanie klawisza myszy uruchamiajacego menu
	// GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON.
    glutAttachMenu( GLUT_RIGHT_BUTTON );


	Initialize();

	glutMainLoop();


	// Cleaning
	glDeleteProgram( idProgram );
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
