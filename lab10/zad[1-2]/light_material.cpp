#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <math.h>

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

int Window_Width;
int Window_Height;

#include "shader_stuff.hpp"
#include "text.h"

SLight Light1 = {
	glm::vec4(0.4, 0.4, 0.4, 1.0),
	glm::vec4(0.8, 0.8, 0.8, 1.0),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(0.0, 3.0, 0.0, 1.0),
	LIGHT_DISTANCE,
};

glm::vec3 Light_Direction = glm::normalize(glm::vec3(0.2, -0.8f, 1.1f));
glm::vec3 Light_Position = glm::vec3(0.0f, 5.0f, -25.0f);

// macierze rzutowania dla kamery patrzacej z punktu widzenia oswietlenia
glm::mat4 lightProj = glm::ortho(-20.0f, 30.0f, -20.0f, 20.0f, 2.0f, 60.5f);

glm::mat4 lightView = glm::lookAt(Light_Position,
                                  Light_Position+Light_Direction,
                                  glm::vec3( 0.0f, 1.0f,  0.0f));

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

int punkty = 0;
#include "OpenGLProgram.h"
#include "skybox_loader.h"

OpenGLProgram program, programKoliber, programSkybox, programInstancje;
OpenGLProgram shadowmap, shadowmapinstance;
CGround Scene;
OpenGLObject Metal;
OpenGLObject kwiatek;
OpenGLObject Koliber;

CSceneObject postac;

OpenGLObjectInstanced Tree, kwiatki;

SkyBoxObject skybox;

const unsigned int DepthMap_Width = 1024, DepthMap_Height = 1024;
GLuint DepthMap_FrameBuffer;

float light_rotation = 0.0f;
int timebase, currenttime = glutGet(GLUT_ELAPSED_TIME);
int frame = 0;
char txt[255];
char pts[255];
const int LICZBA_DRZEW = 50;
const int LICZBA_KWIATKOW = 50;

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

        Metal.rotateY(0.01);

        shadowmapinstance.ClearBuffer();
        shadowmap.ClearBuffer();

        shadowmap.ActivateProgram();

            GLuint idShadowMap = shadowmap.SendProgramId();

            Scene.DrawToShadowMap(idShadowMap);
            kwiatek.DrawToShadowMap(idShadowMap);
            postac.DrawToShadowMap(idShadowMap);
            Metal.DrawToShadowMap(idShadowMap);
            Tree.DrawToShadowMap(idShadowMap);
            kwiatki.DrawToShadowMap(idShadowMap);


        shadowmap.DeactiveProgram();

        shadowmapinstance.ActivateProgram();
            GLuint idShadowMapInstance = shadowmapinstance.SendProgramId();
            //Tree.DrawToShadowMap(idShadowMapInstance);

        shadowmapinstance.DeactiveProgram();

        GLuint shadowmapTexture = shadowmap.getDepthMapTexture();
        GLuint shadowmapTextureInstance = shadowmapinstance.getDepthMapTexture();

        glViewport(0, 0, Window_Width, Window_Height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        program.ActivateProgram();

            Scene.DrawOnScreen(shadowmapTexture);
            postac.DrawOnScreen(shadowmapTexture);
            kwiatek.DrawOnScreen(shadowmapTexture);
            Metal.DrawOnScreen(shadowmapTexture);

        program.DeactiveProgram();

        programInstancje.ActivateProgram();

            kwiatki.DrawOnScreen(shadowmapTextureInstance);
            Tree.DrawOnScreen(shadowmapTextureInstance);

        programInstancje.DeactiveProgram();


        programSkybox.ActivateProgram();

            skybox.DrawSkybox();

        programSkybox.DeactiveProgram();


        programKoliber.ActivateProgram();

            Koliber.getSkyBoxTexture(skybox.getSkyBoxTexture());
            Koliber.getMaterials(Material3);
            Koliber.DrawWithSkyBox();

        programKoliber.DeactiveProgram();


            {
                frame++;
                currenttime = glutGet(GLUT_ELAPSED_TIME);
                if (currenttime - timebase > 1000)
                {
                    sprintf(txt, "FPS: %4.2f", frame*1000.0/(currenttime-timebase));
                    timebase = currenttime;
                    frame=0;
                }
                DrawText8x16( 3, 21, txt, glm::vec4(1.0, 1.0, 0.0, 1.0) );


                sprintf(pts, "Punkty: %d", punkty);
                DrawText8x16( 3, 42, pts, glm::vec4(1.0, 1.0, 0.0, 1.0) );
            }

    glutSwapBuffers();
}


// ---------------------------------------
void Reshape( int width, int height )
{

    Window_Width = width;
	Window_Height = height;

    glViewport( 0, 0, width, height );
	matProj = glm::perspectiveFov(glm::radians(60.0f), (float)width, (float)height, 0.1f, 100.f );
}


// ---------------------------------------------------
void Initialize()
{

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	InitDrawText();

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	program.CreateProgram();
	program.LoadShaders("vertex/vertex_shadow.glsl", "fragment/fragment_shadow.glsl");
	GLuint id = program.SendProgramId();

	shadowmap.CreateProgram();
	shadowmap.LoadShaders("vertex/depthmap.vertex.glsl", "fragment/depthmap.fragment.glsl");
	GLuint idShadowMap = shadowmap.SendProgramId();
	shadowmap.CreateShadowMap();

	shadowmapinstance.CreateProgram();
	shadowmapinstance.LoadShaders("vertex/depthmap_instance.vertex.glsl", "fragment/depthmap.fragment.glsl");
	GLuint idShadowMapInstance = shadowmapinstance.SendProgramId();
	shadowmapinstance.CreateShadowMap();

	Scene.SetProgramId(id);
    Scene.Load("objfiles/scene-levels.obj");
    Scene.CreateTexture("textures/chess.bmp");
    Scene.CreateFromOBJ();

    Metal.SetProgramId(id);
    Metal.Load("objfiles/object.obj");
    Metal.CreateTexture("textures/metal.jpg");
    Metal.move(17.0, 1.0, -17.0);

    postac.SetProgramId(id);
    postac.Load("objfiles/lego.obj");
    postac.CreateTexture("textures/lego.jpg");
    postac.Init(&Scene);

	kwiatek.SetProgramId(id);
    kwiatek.Load("objfiles/flower.obj");
    kwiatek.CreateTexture("textures/flower.png");
    kwiatek.move(10.0, 0.0);


    programInstancje.CreateProgram();
	programInstancje.LoadShaders("vertex/vertex_shadow_instance.glsl", "fragment/fragment_shadow.glsl");
	GLuint idInstacje = programInstancje.SendProgramId();

    Tree.SetProgramId(idInstacje);
    Tree.ChangeInstanceNumber(LICZBA_DRZEW);
    Tree.Init(&Scene);
    Tree.RandomizeLocationOfInstances(2000, 60);
    Tree.Load("objfiles/tree.obj");
    Tree.CreateTexture("textures/grass.bmp");

    kwiatki.SetProgramId(idInstacje);
    kwiatki.ChangeInstanceNumber(LICZBA_KWIATKOW);
    kwiatki.Init(&Scene);
    kwiatki.RandomizeLocationOfInstances(2000, 60, 0.7);
    kwiatki.Load("objfiles/flower.obj");
    kwiatki.CreateTexture("textures/flower.png");

	programSkybox.CreateProgram();
	programSkybox.LoadShaders("vertex/SkyBox-vertex.glsl", "fragment/SkyBox-fragment.glsl");
	GLuint idSkybox = programSkybox.SendProgramId();
    skybox.SetProgramId(idSkybox);
    skybox.CreateSkyBox();

    programKoliber.CreateProgram();
    programKoliber.LoadShaders("vertex/vertex_env.glsl", "fragment/fragment_env.glsl");
	GLuint idKoliber = programKoliber.SendProgramId();
    Koliber.SetProgramId(idKoliber);
	Koliber.Load("objfiles/koliber.obj");
	Koliber.CreateTexture("textures/koliber.bmp");
	Koliber.move(10.0, 2.0, -0.7);
	Koliber.scale(2, 2, 2);
	Koliber.rotateX(1);

}

// ---------------------------------------------------
void Animation(int frame)
{
	glutPostRedisplay();
	glutTimerFunc(10, Animation, 0);
}

void Keyboard( unsigned char key, int x, int y )
{
    float move_vec = 0.2;
    std::vector<OpenGLObjectInstanced> lista_obiektow;
    lista_obiektow.push_back(Tree);
    lista_obiektow.push_back(kwiatki);
    std::vector<int> typy_kolizji = {0, 1};
    switch(key)
    {
		case 27:	// ESC key
			exit(0);
			break;

        case 'w':
            {
                auto [instancja, obiekt] = postac.MoveXZ(0.0, move_vec, lista_obiektow, typy_kolizji);
                if (obiekt < lista_obiektow.size()){
                    //lista_obiektow[obiekt].RemoveInstance(instancja);
                }
                break;
            }

        case 's':
            {
                auto [instancja, obiekt] = postac.MoveXZ(0.0, -move_vec, lista_obiektow, typy_kolizji);
                break;
            }

        case 'd':
            {
                auto [instancja, obiekt] = postac.MoveXZ(-move_vec, 0.0, lista_obiektow, typy_kolizji);
                break;
            }

        case 'a':
            {
                auto [instancja, obiekt] = postac.MoveXZ(move_vec, 0.0, lista_obiektow, typy_kolizji);
                break;
            }

    }
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
