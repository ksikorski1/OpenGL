#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Window_width = 800;
int Window_height = 600;

glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjView;
glm::mat4x4 matModel(1.0);

#include "stuff.h"


#define NUMBER_OF_MESHES 2

enum {
    GROUND = 0,
    SPHERE = 1,
};


// ---------------------------------------
GLuint program;
GLuint vBuffer_pos[NUMBER_OF_MESHES];
GLuint vBuffer_uv[NUMBER_OF_MESHES];
GLuint vArray[NUMBER_OF_MESHES];

std::vector<glm::vec3> OBJ_vertices[NUMBER_OF_MESHES];
std::vector<glm::vec2> OBJ_uvs[NUMBER_OF_MESHES];
std::vector<glm::vec3> OBJ_normals[NUMBER_OF_MESHES];
GLuint TextureID[NUMBER_OF_MESHES];


// ---------------------------------------
class CSceneObject
{

public:

    glm::vec3 Position;
    float radius;


    GLuint VAO;
    int VBO_Size;

    glm::mat4x4 matModel;


    CSceneObject()
    {
        Position.x = rand()%50 - 25;
        Position.y = 0;
        Position.z = rand()%50 - 25;
        radius = 1.0;

        matModel = glm::translate(glm::mat4(1.0), Position);

    }

    void Set(GLuint _vao, int _size)
    {
        VAO = _vao;
        VBO_Size = _size;
    }

    void Draw(GLuint prog)
    {
        glUniformMatrix4fv( glGetUniformLocation( prog, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );

        glBindVertexArray( VAO );
        glDrawArrays( GL_TRIANGLES, 0, VBO_Size );
        glBindVertexArray( 0 );

    }

};


#define NUMBER_OF_OBJECTS 250

int CollisionID = -1;

CSceneObject Table_of_Objects[NUMBER_OF_OBJECTS];


// ---------------------------------------
void DisplayScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Geometria sceny
	matView = glm::translate( glm::mat4( 1.0 ), glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	matProjView = matProj * matView;

    glUseProgram( program );

        glUniformMatrix4fv( glGetUniformLocation( program, "matPV" ), 1, GL_FALSE, glm::value_ptr(matProjView) );
        glUniformMatrix4fv( glGetUniformLocation( program, "matModel" ), 1, GL_FALSE, glm::value_ptr(matModel) );


        // STENCIL: wpisywanie do bufora zer
        glStencilFunc(GL_ALWAYS, 0 , 0xFF);


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


        for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
        {
            // STENCIL: wpisywanie do bufora i+1
            glStencilFunc(GL_ALWAYS, i + 1, 0xFF);

            if (i == CollisionID)
            {
                glUniform1i( glGetUniformLocation( program, "selected" ), 1);
                Table_of_Objects[i].Draw(program);
                glUniform1i( glGetUniformLocation( program, "selected" ), 0);

            }
            else
            {
                 Table_of_Objects[i].Draw(program);
            }

        }

    glUseProgram( 0 );

	glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    Window_width = width;
    Window_height = height;

	glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}

// ---------------------------------------------------
void Initialize()
{

	// tekstura
	int tex_width;
	int tex_height;
	unsigned char *tex_data;

	// Odsuwanie widoku (na potrzeby przykladu)
	_scene_translate_z = -5;
	_scene_translate_y = -3;
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClearStencil(0);


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


	glEnable( GL_DEPTH_TEST );
    glEnable( GL_STENCIL_TEST );

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    for (int i=0; i<NUMBER_OF_OBJECTS; ++i)
    {
        Table_of_Objects[i].Set(vArray[SPHERE], OBJ_vertices[SPHERE].size());
    }

}


// --------------------------------------------------------------
void MouseButton( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {

        _mouse_buttonState = state;

        if( state == GLUT_DOWN )
        {
            _mouse_buttonX = x;
            _mouse_buttonY = y;
        }

        if ( state == GLUT_DOWN )
        {


            GLbyte color[4];
            GLfloat depth;
            GLuint index;


            glReadPixels( x, Window_height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
            glReadPixels( x, Window_height-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );
            glReadPixels( x, Window_height-y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);


            printf("Color: %d %d %d \n", (unsigned char)color[0], (unsigned char)color[1], (unsigned char)color[2]);
            printf("Depth: %f\n", depth );
            printf("Index: %d\n", index );

            CollisionID = index - 1;

        }

    }
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



    }


    glutPostRedisplay();

}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
	// GLUT
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutInitWindowSize( Window_width, Window_height );
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
