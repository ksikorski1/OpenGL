#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "obj_loader.hpp"
#include "texture_loader.hpp"
#include "shader_stuff.h"



glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matModel(1.0);

#define NUMBER_OF_OBJECTS 3

enum {
    GROUND = 0,
    SPHERE = 1,
};




// ---------------------------------------

GLuint program;
GLuint vBuffer_pos[NUMBER_OF_OBJECTS];
GLuint vBuffer_uv[NUMBER_OF_OBJECTS];
GLuint vArray[NUMBER_OF_OBJECTS];

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_OBJECTS];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_OBJECTS];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_OBJECTS];


GLuint TextureID[NUMBER_OF_OBJECTS];


// ----------------------------------------------------------------
class CSceneObject
{

public:

	// pozycja obiektu na scenie
    glm::vec3 Position;
    glm::mat4x4 matModel;

	// potok openGL
    GLuint VAO;
    int VBO_Size;

	// NOWE: srednica sfery otaczajacej obiekt
	float radius = 1.0f;



    CSceneObject()
    {
    }


    // ustawienie potoku
    void Init(GLuint _vao, int _size)
    {
        VAO = _vao;
        VBO_Size = _size;
    }

    // rysowanie na scenie
    void Draw()
    {
		GLint current_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

        glUseProgram(current_program);
        glUniformMatrix4fv( glGetUniformLocation( current_program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

    // ustawienie pozycji na scenie
    void SetPosition(float x, float y, float z)
    {
        Position = glm::vec3(x, y, z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    // zmiana pozycji na scenie
    void MoveXZ(float _x, float _z)
    {
        Position += glm::vec3(_x, 0.0, _z);
        matModel = glm::translate(glm::mat4(1.0), Position);
    }

    void MoveXZ(float _x, float _z, const CSceneObject &_other)
    {
        glm::vec3 OldPosition = Position;

        Position += glm::vec3(_x, 0.0, _z);

        if(this->isCollision(_other)){
            Position = OldPosition;
        }
        printf("Kolizja \n");

        matModel = glm::translate(glm::mat4(1.0), Position);
    }


	// NOWE: sprawdzanie kolizji z innym obiektem
    bool isCollision(const CSceneObject &_other)
    {
        float distance = glm::distance(this->Position, _other.Position);
        if (distance < this->radius + _other.radius)
        {
            return true;
        }
        return false;
    }


};




// Obiekty na scenie
CSceneObject Stone;
CSceneObject myCharacter;




// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Geometria sceny
	matView = glm::translate( glm::mat4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );

	matModel = glm::mat4x4( 1.0 );


    glUseProgram( program );

		// Wyslanie macierzy rzutowania
		glUniformMatrix4fv( glGetUniformLocation( program, "matProj" ), 1, GL_FALSE, glm::value_ptr(matProj) );
		glUniformMatrix4fv( glGetUniformLocation( program, "matView" ), 1, GL_FALSE, glm::value_ptr(matView) );
		glUniformMatrix4fv( glGetUniformLocation( program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );



		// GROUND
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);
		glUniform1i(glGetUniformLocation(program, "tex0"), 0);

		glBindVertexArray( vArray[GROUND] );
		glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices[GROUND].size() );
		glBindVertexArray( 0 );


		// SPHERE
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
		glUniform1i(glGetUniformLocation(program, "tex0"), 0);

		// Rendering obiektu statycznego
		Stone.Draw();

		// Rendering postaci, ktora sie poruszamy
		myCharacter.Draw();


    glUseProgram( 0 );

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// --------------------------------------------------------------
void Keyboard( unsigned char key, int x, int y )
{
    float move_vec = 0.2;

    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

        case 'w':
            myCharacter.MoveXZ(move_vec, 0.0f, Stone);
            break;
         case 's':
            myCharacter.MoveXZ(-move_vec, 0.0f, Stone);
            break;

         case 'd':
            myCharacter.MoveXZ(0.0f, move_vec, Stone);
            break;
         case 'a':
            myCharacter.MoveXZ(0.0f, -move_vec, Stone);
            break;

    }

    glutPostRedisplay();
}

// ---------------------------------------------------
void Initialize()
{

	// tekstury
	int tex_width;
	int tex_height;
	unsigned char *tex_data;



	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -8;
	_scene_translate_y = -3;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );


	// Programowanie potoku
	program = glCreateProgram();

	glAttachShader( program, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader( program, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram( program );




	// GROUND
	if (!loadOBJ("chess.obj", OBJ_vertices[GROUND], OBJ_uvs[GROUND], OBJ_normals[GROUND]))
	{
		printf("Not loaded!\n");
		exit(1);
	}
	// Vertex arrays
	glGenVertexArrays( 1, &vArray[GROUND] );
	glBindVertexArray( vArray[GROUND] );

	glGenBuffers( 1, &vBuffer_pos[GROUND] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[GROUND] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[GROUND].size() * sizeof(glm::vec3), &(OBJ_vertices[GROUND])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[GROUND] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[GROUND] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[GROUND].size() * sizeof(glm::vec2), &(OBJ_uvs[GROUND])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );


	loadBMP_custom("chess.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[GROUND]);
	glBindTexture(GL_TEXTURE_2D, TextureID[GROUND]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



    // SPHERE
    if (!loadOBJ("sphere.obj", OBJ_vertices[SPHERE], OBJ_uvs[SPHERE], OBJ_normals[SPHERE]))
	{
		printf("Not loaded!\n");
		exit(1);
	}

	// Vertex arrays
	glGenVertexArrays( 1, &vArray[SPHERE] );
	glBindVertexArray( vArray[SPHERE] );

	glGenBuffers( 1, &vBuffer_pos[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_pos[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_vertices[SPHERE].size() * sizeof(glm::vec3), &(OBJ_vertices[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vBuffer_uv[SPHERE] );
	glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv[SPHERE] );
	glBufferData( GL_ARRAY_BUFFER, OBJ_uvs[SPHERE].size() * sizeof(glm::vec2), &(OBJ_uvs[SPHERE])[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, NULL );
	glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );

    loadBMP_custom("grass.bmp", tex_width, tex_height, &tex_data);

	glGenTextures(1, &TextureID[SPHERE]);
	glBindTexture(GL_TEXTURE_2D, TextureID[SPHERE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


    // Inicjalizacja obiektow

    Stone.Init(vArray[SPHERE], OBJ_vertices[SPHERE].size() );
    Stone.SetPosition(5, 0, -5);

    myCharacter.Init(vArray[SPHERE], OBJ_vertices[SPHERE].size() );
    myCharacter.SetPosition(0, 0, 0);

	glEnable( GL_DEPTH_TEST );


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

	glutMainLoop();


	return 0;
}
