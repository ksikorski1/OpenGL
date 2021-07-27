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
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow
GLuint idVBO_color;

float Brightness = 0.0;

// ---------------------------------------
// Wspolrzedne wierzchokow
/*
GLfloat triangles_coord[2*3*2] =
{
	-0.8f, -0.8f,
	 0.0f, -0.8f,
	-0.4f,  0.0f,

	 0.0f,  0.0f,
	 0.8f,  0.0f,
	 0.4f,  0.8f
};
*/
// ---------------------------------------
// Wspolrzedne wierzchokow
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
	0.f, 1.f, 0.f,
	1.f, 0.f, 0.f,
	0.f, 0.f, 0.f,

	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,

	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f
};
#define NUMBER_OF_TRIANGLES 10

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

// ---------------------------------------
void DisplayScene()
{

	glClear( GL_COLOR_BUFFER_BIT );

	// Wlaczenie VAO i programu
	glBindVertexArray( idVAO );

	glUseProgram( idProgram );
	//wysylamy pod zmienna uniform o nazwie jasnosc zawartosc zmiennej Brightness
	glUniform1f( glGetUniformLocation( idProgram, "Jasnosc" ), Brightness );
	// Generowanie obiektow na ekranie
	glDrawArrays( GL_TRIANGLES, 0, NUMBER_OF_TRIANGLES*3 );


	//tak samo dla 2 programu

	glUseProgram( idProgram2 );
	// Generowanie obiektow na ekranie
	// jeden z trojkatow nie bedzie reagowal na jasnosc - ten wyswietlany przez 2 potok graficzny
	glDrawArrays( GL_TRIANGLES, 0, 1*3 );


	// Wylaczanie
	glUseProgram( 0 );
	glBindVertexArray( 0 );


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
    // kolory okienka
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );


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


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

    //tworzy losowa tablice wspolrzednych o liczbie trojkatow rownej NUMBER_OF_TRIANGLES
    random_triangles();

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );

	//ZMIENIONE ABY UZYWAC LOSOWEJ TABLICY WIERZCHOLKOW
	glBufferData( GL_ARRAY_BUFFER, sizeof( random_triangle_array ), random_triangle_array, GL_STATIC_DRAW );
	//glBufferData( GL_ARRAY_BUFFER, sizeof( random_triangle_array ), random_triangle_array, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

    //bufor na kolory
	glGenBuffers( 1, &idVBO_color );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color );

	glBufferData( GL_ARRAY_BUFFER, sizeof( triangles_color ), triangles_color, GL_STATIC_DRAW );

    //tablica wierzcholkow - 1 atrybut (0 juz zajety), druga skladowa to ilosc - 3 bo r g b
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
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
