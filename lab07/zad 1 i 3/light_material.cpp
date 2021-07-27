#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define __CHECK_FOR_ERRORS 	{GLenum errCode; if ((errCode = glGetError()) != GL_NO_ERROR) printf("Error (%d): %s in file %s at line %d !\n", errCode, gluErrorString(errCode), __FILE__,  __LINE__);}

#include "obj_loader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ---------------------------------------
// NOWE : OSWIETLENIE - parametry
// ---------------------------------------

enum
{
    LIGHT_PHONG = 0,
    LIGHT_GOURAUD,
    LIGHT_NUMBER
};
int MODE = LIGHT_PHONG;

#include "shader_stuff.hpp"

SLight Light1 = {
	glm::vec4(0.4, 0.4, 0.4, 1.0),
	glm::vec4(0.8, 0.8, 0.8, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	LIGHT_DISTANCE,
};


// ---------------------------------------
// NOWE : MATERIALY - parametry
// ---------------------------------------
typedef struct SMaterial
{

	glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    GLfloat Shininess;

} SMaterial;


SMaterial Material1 = {
	glm::vec4(0.2f, 0.8f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
	32.0f
};

SMaterial Material2 = {
	glm::vec4(1.0f, 0.2f, 0.2f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
	32.0f
};

SMaterial Material3 = {
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
	32.0f
};


glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::vec3 Camera_Position;
#include "OpenGLProgram.h"


OpenGLProgram Scene, Tree;
OpenGLProgram Scene_gouraud, Tree_gouraud;
OpenGLProgram Niebo, Wieza;
OpenGLProgram Niebo_gouraud, Wieza_gouraud;

OpenGLProgram kwiatek;

float light_rotation = 0.0f;
// ---------------------------------------
void DisplayScene()
{

	// Czyszczenie ramki
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // ------------------------------
	// Geometria sceny
	matView = glm::mat4x4( 1.0 );
	matView = glm::translate( matView, glm::vec3( _scene_translate_x, _scene_translate_y, _scene_translate_z ) );
	matView = glm::rotate( matView, _scene_rotate_x, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	matView = glm::rotate( matView, _scene_rotate_y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	Camera_Position = ExtractCameraPos(matView);

		{
		    light_rotation += 0.00005f;

		    glm::mat4 matModelLight = glm::mat4(1.0f);
            matModelLight = glm::rotate(matModelLight, light_rotation, glm::vec3(0.0, 1.0, 0.0));
            matModelLight = glm::translate(matModelLight, glm::vec3(5.0, 0.0, 0.0));

            glm::vec4 LightBasePosition = glm::vec4(0.0, 3.0, 0.0, 1.0);
            Light1.Position = matModelLight * LightBasePosition;
            Light1.Attenuation = LIGHT_DISTANCE;
		}
        __CHECK_FOR_ERRORS
		// Pierwszy obiekt
        if (MODE == 0){
            {
                Scene.getMaterials(Material2);
                Scene.Draw();
                __CHECK_FOR_ERRORS
            }

            // Drugi obiekt
            {
                Tree.getMaterials(Material2);
                Tree.Draw();
                __CHECK_FOR_ERRORS
            }
            {
                Wieza.getMaterials(Material3);
                Wieza.Draw();
            }
            {
                Niebo.getMaterials(Material3);
                Niebo.Draw();
            }
        }


		//----------------------------
		//Inny model oswietlenia
        // Pierwszy obiekt
        if (MODE == 1){
            {
                Scene_gouraud.getMaterials(Material3);
                Scene_gouraud.Draw();
            }

            // Drugi obiekt
            {
                Tree_gouraud.getMaterials(Material2);
                Tree_gouraud.Draw();
            }

            {
                Wieza.getMaterials(Material3);
                Wieza.Draw();
            }
            {
                Niebo.getMaterials(Material3);
                Niebo.Draw();
            }
        }

        kwiatek.getMaterials(Material3);
        kwiatek.Draw();


    glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{
    glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}


// ---------------------------------------------------
void Initialize()
{

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    Scene.setProgramId();
    Tree.setProgramId();
    Niebo.setProgramId();
    Wieza.setProgramId();

	Scene.loadShaders("vertex/vertex.glsl", "fragment/fragment2.glsl");
	Tree.loadShaders("vertex/vertex.glsl", "fragment/fragment2.glsl");
	Niebo.loadShaders("vertex/vertex.glsl", "fragment/fragment2.glsl");
	Wieza.loadShaders("vertex/vertex.glsl", "fragment/fragment2.glsl");

	// Ladowanie obiektow
	Scene.Load("objfiles/scene.obj");
	Tree.Load("objfiles/object.obj");
	Niebo.Load("objfiles/sky.obj");
	Wieza.Load("objfiles/tower.obj");


	Scene.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");
	Tree.CreateTexture("textures/metal.jpg");
	Niebo.CreateTexture("textures/sky.jpg");
	Wieza.CreateTexture("textures/metal.jpg");

    Scene.move(3.0, 0.0, 0.0);
	Tree.move(0.0, -1.0, 0.0);
	Scene.scale(2.0, 2.0, 2.0);
	Niebo.move(0.0, -2.0);
	Niebo.scale(25.0, 25.0, 25.0);
	Wieza.move(-7.0, -2.0, -15.0);
	Wieza.scale(1.5, 1.5, 1.5);
	__CHECK_FOR_ERRORS

	//-----------------------------------------------------------------------
	//Drugi model oswietlenia

	Scene_gouraud.setProgramId();
    Tree_gouraud.setProgramId();
    Niebo_gouraud.setProgramId();
    Wieza_gouraud.setProgramId();

	Scene_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");
	Tree_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");
	Niebo_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");
	Wieza_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");

	// Ladowanie obiektow
	Scene_gouraud.Load("objfiles/scene.obj");
	Tree_gouraud.Load("objfiles/object.obj");
	Niebo_gouraud.Load("objfiles/sky.obj");
	Wieza_gouraud.Load("objfiles/tower.obj");

	Scene_gouraud.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");
	Tree_gouraud.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");
	Niebo_gouraud.CreateTexture("textures/sky.jpg");
	Wieza_gouraud.CreateTexture("textures/metal.jpg");

    Scene_gouraud.move(3.0, 0.0, 0.0);
    Tree_gouraud.move(0.0, -1.0, 0.0);
	Scene_gouraud.scale(2.0, 2.0, 2.0);
	Niebo_gouraud.move(0.0, -2.0);
	Niebo_gouraud.scale(25.0, 25.0, 25.0);
	Wieza_gouraud.move(-7.0, -2.0, -15.0);
	Wieza_gouraud.scale(1.5, 1.5, 1.5);

    kwiatek.setProgramId();
    kwiatek.loadShaders("vertex/vertex2.glsl", "fragment/fragment2.glsl");
    kwiatek.Load("objfiles/flower.obj");
    kwiatek.CreateTexture("textures/flower.png");
    kwiatek.move(0.0, 2.0);

}

// ---------------------------------------------------
void Animation(int frame)
{
	glutPostRedisplay();
	glutTimerFunc(10, Animation, 0);
}

// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    // GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutInitWindowSize( 800, 600 );
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

	glutTimerFunc(10, Animation, 0);

    glutMainLoop();

    // Cleaning();

    return 0;
}
