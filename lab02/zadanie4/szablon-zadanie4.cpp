#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "shader_stuff.h"

// ---------------------------------------
// Identyfikatory obiektow

GLuint idProgram;	// id programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord;	// bufora wierzcholkow
GLuint idVBO_color;
GLuint idVBO_size;

float Brightness = 0.0;

#define NUMBER_OF_POINTS 500

GLfloat r2()
{
    //daje liczbe miedzy [-1, 1]
    GLfloat random_value = (GLfloat)rand()/RAND_MAX * 2.0-1.0;
    return random_value;
}

//*2 bo x y
GLfloat random_point_array[NUMBER_OF_POINTS * 2];
//*3 bo r g b - powinno byc 4 (rgba)
GLfloat random_point_color[NUMBER_OF_POINTS * 3];
//size to po prostu liczba
GLfloat random_point_size[NUMBER_OF_POINTS];

//tworzy punkty o losowych wspolrzednych, kolorach i rozmiarze
void random_points()
{
    srand ( time ( NULL));
    //najpierw wspolrzedne
    for (int i = 0; i < (NUMBER_OF_POINTS * 2); i++){
        random_point_array[i] = r2();
    }
    //potem kolory - abs bo zle mi sie patrzy na ujemne kolory :)
    for (int j = 0; j < (NUMBER_OF_POINTS * 3); j++){
        random_point_color[j] = abs(r2());
        //printf("%f\n", random_point_color[j]);
    }
    //a na koniec rozmiar - *10 aby nie byly takie malutkie
    for (int k = 0; k < NUMBER_OF_POINTS; k++){
        random_point_size[k] = r2()* 10;
    }
}

// ---------------------------------------
void DisplayScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

	// Wlaczenie VAO i programu
	glBindVertexArray(idVAO);
	glUseProgram(idProgram);

	//wysylamy pod zmienna uniform o nazwie jasnosc zawartosc zmiennej Brightness
	glUniform1f(glGetUniformLocation(idProgram, "Jasnosc"), Brightness);

	// Generowanie obiektow na ekranie - tutaj punkty
	glDrawArrays(GL_POINTS, 0, NUMBER_OF_POINTS * 2);

	// Wylaczanie
	glUseProgram(0);
	glBindVertexArray(0);

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
    // kolory okienka
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_PROGRAM_POINT_SIZE);

    random_points();

	// 1. Program i shadery
	idProgram = glCreateProgram();
	glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, "vertex-4.glsl"));
	glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment-4.glsl"));
	LinkAndValidateProgram( idProgram );


	// 2. Vertex arrays
	glGenVertexArrays( 1, &idVAO );
	glBindVertexArray( idVAO );

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers( 1, &idVBO_coord );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );

	//ZMIENIONE ABY UZYWAC LOSOWEJ TABLICY WIERZCHOLKOW
	glBufferData( GL_ARRAY_BUFFER, sizeof( random_point_array ), random_point_array, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

    //bufor na kolory
	glGenBuffers( 1, &idVBO_color );
	glBindBuffer( GL_ARRAY_BUFFER, idVBO_color );
	glBufferData( GL_ARRAY_BUFFER, sizeof( random_point_color ), random_point_color, GL_STATIC_DRAW );

    //tablica wierzcholkow - 1 atrybut (0 juz zajety), druga skladowa to ilosc - 3 bo r g b
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );

	//bufor na rozmiar punktow
	glGenBuffers(1, &idVBO_size);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_size);
	glBufferData(GL_ARRAY_BUFFER, sizeof(random_point_size), random_point_size, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

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
	glDeleteVertexArrays( 1, &idVBO_coord );
	glDeleteVertexArrays( 1, &idVAO );

	return 0;
}
