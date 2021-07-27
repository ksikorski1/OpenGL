#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_stuff.hpp"
#include "texture_loader.hpp"


// ---------------------------------------
GLuint program;
GLuint vArray;

GLuint TextureID;

// ---------------------------------------
glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjView;



// ---------------------------------------
GLfloat vertices_pos[] =
{
    -1.0f, -1.0f, 0.0f,	// bottom left
	 1.0f, -1.0f, 0.0f,	// bottom right
     0.0f,  1.0f, 0.0f, // upper
};


GLfloat vertices_color[] =
{
	1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};


GLfloat vertices_tex[] =
{
	0.0f, 0.0f, // for bottom left vertex
	1.0f, 0.0f, // for bottom right vertex
	0.5f, 1.0f, // for upper vertex
};


// ------------------------------------------------
// NOWE: ANISOTROPY - parametry
GLfloat maxAnisotropy = -1.0f;
bool Anisotropy = false;
// ------------------------------------------------



// ------------------------------------------------
void DisplayScene()
{

	// Czyszczenie ramki (jak zwykle)
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	// Geometria sceny  (jak zwykle)
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	matProjView = matProj * matView;


	glBindVertexArray( vArray );
	glUseProgram( program );

		glUniformMatrix4fv( glGetUniformLocation( program, "matProjView" ), 1, GL_FALSE, &(matProjView[0][0]) );


		// ------------------------------------------------
		// NOWE: Wlaczanie/aktywowanie tekstur
		// ------------------------------------------------
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		if( maxAnisotropy > 0.0f )
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (Anisotropy == true)?maxAnisotropy:1.0f );
		}

		glUniform1i(glGetUniformLocation(program, "tex0"), 0);
		glDrawArrays( GL_TRIANGLES, 0, 3*3 );



	glUseProgram( 0 );
	glBindVertexArray( 0 );



	glutSwapBuffers();
}





// ---------------------------------------------------
void CreateTexture()
{
	int tex_width;
	int tex_height;
	unsigned char *tex_data;

	loadBMP_custom("uvtemplate.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);


}

// ---------------------------------------------------
void Initialize()
{

	CreateTexture();

	// ------------------------------------------------
	// ANISOTROPY: pobranie maksymalnego stopnia anizotropii
	// ------------------------------------------------
    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );

    if( glGetError() != GL_NO_ERROR )
	{
		printf("Error: GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT;\n");
		maxAnisotropy = -1.0;
	}
	else
	{
		printf("Max Anisotropy: %f\n", maxAnisotropy);
	}



	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Programowanie potoku
	program = glCreateProgram();
	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram( program );

	// Vertex arrays
	glGenVertexArrays( 1, &vArray );
	glBindVertexArray( vArray );

	// Wspolrzedne wierzchokow
	GLuint vBuffer_pos;
	glGenBuffers( 1, &vBuffer_pos );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	// Wspolrzedne textury UV
	GLuint vBuffer_uv;
	glGenBuffers( 1, &vBuffer_uv );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex, GL_STATIC_DRAW );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 2 );

	glEnable( GL_DEPTH_TEST );





}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Menu( int value )
{
    switch( value )
    {
		case 0:
			glutLeaveMainLoop();
			break;

		case 1:
			Anisotropy = 0;
			break;

		case 2:
			Anisotropy = 1;
			break;
    }

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
	glutInitWindowSize( 500, 500 );
	glutCreateWindow( "OpenGL" );

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
	glutDisplayFunc( DisplayScene );
	glutReshapeFunc( Reshape );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutMouseWheelFunc( MouseWheel );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKeys );

	glutCreateMenu( Menu );
    glutAddMenuEntry( "Anisotropy OFF", 1 );
    glutAddMenuEntry( "Anisotropy ON", 2 );
    glutAddMenuEntry( "Koniec", 0 );
    glutAttachMenu( GLUT_RIGHT_BUTTON );


	glutMainLoop();

	// Cleaning

	return 0;
}
