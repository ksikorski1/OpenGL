#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	glm::vec4(0.3, 0.3, 0.3, 1.0),
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


glm::mat4x4 matProj;
glm::mat4x4 matView;
glm::mat4x4 matProjViewModel;
glm::vec3 Camera_Position;
#include "object3d.hpp"


OpenGLProgram Scene, Tree;
OpenGLProgram Scene_gouraud, Tree_gouraud;

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

		// Pierwszy obiekt
        if (MODE == 0){
            {
                Scene.getMaterials(Material1);
                Scene.Draw();
            }

            // Drugi obiekt
            {
                Tree.getMaterials(Material2);
                Tree.Draw();
            };
        }


		//----------------------------
		//Inny model oswietlenia
        // Pierwszy obiekt
        if (MODE == 1){
            {
                Scene_gouraud.getMaterials(Material1);
                Scene_gouraud.Draw();
            }

            // Drugi obiekt
            {
                Tree_gouraud.getMaterials(Material2);
                Tree_gouraud.Draw();
            };
        }




	glUseProgram( 0 );

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

    Scene.setProgramId();
    Tree.setProgramId();

	Scene.loadShaders("vertex/vertex.glsl", "fragment/fragment.glsl");
	Tree.loadShaders("vertex/vertex.glsl", "fragment/fragment.glsl");

	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// Ladowanie obiektow
	Scene.Load("objfiles/scene.obj");
	Tree.Load("objfiles/object.obj");

	Scene.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");
	Tree.CreateTexture("textures/metal.jpg");

	//-----------------------------------------------------------------------
	//Drugi model oswietlenia

	Scene_gouraud.setProgramId();
    Tree_gouraud.setProgramId();

	Scene_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");
	Tree_gouraud.loadShaders("vertex/gouraud_vertex.glsl", "fragment/gouraud_fragment.glsl");

	// Ladowanie obiektow
	Scene_gouraud.Load("objfiles/scene.obj");
	Tree_gouraud.Load("objfiles/object.obj");

	Scene_gouraud.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");
	Tree_gouraud.CreateTexture("textures/41832706361b9823c8cco_swnqu.jpg");

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
