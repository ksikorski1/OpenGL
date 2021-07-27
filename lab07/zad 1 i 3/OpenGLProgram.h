#ifndef __OpenGLProgram
#define __OpenGLProgram

class OpenGLProgram
{

public:

    GLuint idProg;
	std::vector<glm::vec3> OBJ_vertices;
	std::vector<glm::vec2> OBJ_uvs;
	std::vector<glm::vec3> OBJ_normals;
	glm::mat4 matModel = glm::mat4(1.0);
	glm::mat4 matProjViewMode;

    SMaterial ObjectMaterial;

	GLuint vArray;
	GLuint Size;

	GLuint TextureID;
	int tex_width;
    int tex_height;
    int n;
    unsigned char *data;

	OpenGLProgram() { }

	void setProgramId(){
        idProg = glCreateProgram();
    }

	void Draw()
	{
	    glUseProgram(idProg);

            sendMaterials();
            sendLightning();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            glUniform1i(glGetUniformLocation(idProg, "tex0"), 0);

            glBindVertexArray( vArray );
                glDrawArrays( GL_TRIANGLES, 0, Size );

            glBindVertexArray( 0 );
		glUseProgram( 0 );

	}

	// ---------------------------------------
	void Load(char *filename)
	{

		if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
		{
			printf("Not loaded!\n");
			exit(1);
		}

		// Liczba wierzcholkow potrzebna do wyrenderowania obiektu
		Size = OBJ_vertices.size();

		// Vertex arrays
		glGenVertexArrays( 1, &this->vArray );
		glBindVertexArray( this->vArray );

		// Wspolrzedne wierzchokow
		GLuint vBuffer_coord;
		glGenBuffers( 1, &vBuffer_coord );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_coord );
		glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 0 );

		// Wektory normalne
		GLuint vBuffer_normal;
		glGenBuffers( 1, &vBuffer_normal );
		glBindBuffer( GL_ARRAY_BUFFER, vBuffer_normal );
		glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
		glEnableVertexAttribArray( 1 );

        //uv
		GLuint vBuffer_uv;
		glGenBuffers( 1, &vBuffer_uv );
        glBindBuffer( GL_ARRAY_BUFFER, vBuffer_uv );
        glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 2 );


        __CHECK_FOR_ERRORS
		glBindVertexArray( 0 );

	}

	void sendMaterials() {

        // (a) material
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Ambient"), 1, &ObjectMaterial.Ambient[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Diffuse"), 1, &ObjectMaterial.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myMaterial.Specular"), 1, &ObjectMaterial.Specular[0]);
		glUniform1f( glGetUniformLocation(idProg, "myMaterial.Shininess"), ObjectMaterial.Shininess);

		// (b) macierz rzutowania
		matProjViewMode = matProj * matView * matModel;

		glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

		glUniformMatrix4fv( glGetUniformLocation( idProg, "matProjViewModel" ), 1, GL_FALSE, &(matProjViewMode[0][0]) );
		glUniformMatrix4fv( glGetUniformLocation( idProg, "matModel" ), 1, GL_FALSE, &(matModel[0][0]) );
		glUniformMatrix3fv( glGetUniformLocation( idProg, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );
    }

    void sendLightning(){

        glUniform3fv( glGetUniformLocation( idProg, "Camera_Position" ), 1, &Camera_Position[0] );
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Ambient"), 1, &Light1.Ambient[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Specular"), 1, &Light1.Specular[0]);
		glUniform4fv( glGetUniformLocation(idProg, "myLight.Position"), 1, &Light1.Position[0]);
		glUniform1f( glGetUniformLocation(idProg, "myLight.Attenuation"), Light1.Attenuation);
    }

    void getMaterials(SMaterial Material){
        ObjectMaterial = Material;
    }

    void loadShaders(char* vertex_filename, char* fragment_filename){
        glAttachShader( idProg, LoadShader(GL_VERTEX_SHADER, vertex_filename));
        glAttachShader( idProg, LoadShader(GL_FRAGMENT_SHADER, fragment_filename));

        LinkAndValidateProgram( idProg );
    }
    void CreateTexture(char* filename){

        //stbi_set_flip_vertically_on_load(true);

        data = stbi_load(filename, &tex_width, &tex_height, &n, STBI_rgb_alpha);

        glGenTextures(1, &TextureID);
        glBindTexture(GL_TEXTURE_2D, TextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // (b) Zachowanie tekstury w przypadku powiekszenia

        // GL_LINEAR, GL_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // (c) Zachowanie tekstury w przypadku pomniejszenia

        // GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        // Generowanie mipmap automatycznie
        glGenerateMipmap(GL_TEXTURE_2D);

        // Podstawowy level mipmap (default 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    }
    void move(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
        matModel = glm::translate( matModel, glm::vec3( X, Y, Z) );
    }

    void rotateX(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 1.0f, 0.0f, 0.0f ) );
    }
    void rotateY(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    }
    void rotateZ(float angle){
        matModel = glm::rotate( matModel, angle, glm::vec3( 0.0f, 0.0f, 1.0f ) );
    }
    void scale(float X = 0.0f, float Y = 0.0f, float Z = 0.0f){
        matModel = glm::scale(matModel, glm::vec3(X, Y, Z));
    }

};


#endif
