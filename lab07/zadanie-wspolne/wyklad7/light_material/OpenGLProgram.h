class OpenGLProgram
{
    public:
        glm::mat4 ProjectionMatrix;
        GLuint idProgram;

        GLuint idVAO;
        GLuint idVBO_coord;
        GLuint idVBO_normal;
        GLuint idVBO_uv;

        glm::mat4 matModel = glm::mat4x4( 1.0 );

        std::vector<glm::vec3> OBJ_vertices;
        std::vector<glm::vec2> OBJ_uvs;
        std::vector<glm::vec3> OBJ_normals;

        GLuint TextureID;
        int tex_width;
        int tex_height;
        int n;
        unsigned char *data;

        void CreateProgram(){
            idProgram = glCreateProgram();
        }

        void loadShaders(char* vertex_filename, char* fragment_filename){
            glAttachShader( idProgram, LoadShader(GL_VERTEX_SHADER, vertex_filename));
            glAttachShader( idProgram, LoadShader(GL_FRAGMENT_SHADER, fragment_filename));

            LinkAndValidateProgram( idProgram );
        }

        void ImportOBJ(char* filename){


            if (!loadOBJ(filename, OBJ_vertices, OBJ_uvs, OBJ_normals))
            {
                printf("Not loaded!\n");
                exit(1);
            }
            CreateVBO();
        }

        void CreateVBO(){
            glGenVertexArrays( 1, &idVAO );
            glBindVertexArray( idVAO );

            // Bufor na wspolrzedne wierzcholkow
            glGenBuffers( 1, &idVBO_coord );
            glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
            glBufferData( GL_ARRAY_BUFFER, OBJ_vertices.size() * sizeof(glm::vec3), &OBJ_vertices[0], GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 0 );


            glGenBuffers( 1, &idVBO_normal );
            glBindBuffer( GL_ARRAY_BUFFER, idVBO_normal );
            glBufferData( GL_ARRAY_BUFFER, OBJ_normals.size() * sizeof(glm::vec3), &OBJ_normals[0], GL_STATIC_DRAW );
            glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 1 );


            glGenBuffers( 1, &idVBO_uv );
            glBindBuffer( GL_ARRAY_BUFFER, idVBO_uv );
            glBufferData( GL_ARRAY_BUFFER, OBJ_uvs.size() * sizeof(glm::vec2), &OBJ_uvs[0], GL_STATIC_DRAW );
            glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );
            glEnableVertexAttribArray( 2 );


            glBindVertexArray( 0 );
        }

        void Draw(){
            glBindVertexArray( idVAO );
            glUseProgram( idProgram );

                glUniform3fv( glGetUniformLocation( idProgram, "Camera_Position" ), 1, &Camera_Position[0] );
                // Generowanie obiektow na ekranie

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TextureID);

                // NOWE: Przekazanie identyfikatora samplera
                glUniform1i(glGetUniformLocation(idProgram, "tex0"), 0);

                glUniform4fv( glGetUniformLocation(idProgram, "myLight.Ambient"), 1, &Light1.Ambient[0]);
                glUniform4fv( glGetUniformLocation(idProgram, "myLight.Diffuse"), 1, &Light1.Diffuse[0]);
                glUniform4fv( glGetUniformLocation(idProgram, "myLight.Specular"), 1, &Light1.Specular[0]);
                glUniform4fv( glGetUniformLocation(idProgram, "myLight.Position"), 1, &Light1.Position[0]);
                glUniform1f( glGetUniformLocation(idProgram, "myLight.Attenuation"), Light1.Attenuation);

                glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Ambient"), 1, &Material1.Ambient[0]);
                glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Diffuse"), 1, &Material1.Diffuse[0]);
                glUniform4fv( glGetUniformLocation(idProgram, "myMaterial.Specular"), 1, &Material1.Specular[0]);
                glUniform1f( glGetUniformLocation(idProgram, "myMaterial.Shininess"), Material1.Shininess);

                glm::mat4 matProjViewModel = ProjectionMatrix * matModel;
                glm::mat3 matNormal = glm::transpose(glm::inverse(glm::mat3(matModel)));

                glUniformMatrix4fv( glGetUniformLocation( idProgram, "matProjViewModel" ), 1, GL_FALSE, &(matProjViewModel[0][0]) );
                glUniformMatrix4fv( glGetUniformLocation( idProgram, "matModel" ), 1, GL_FALSE, &(matModel[0][0]) );
                glUniformMatrix3fv( glGetUniformLocation( idProgram, "matNormal" ), 1, GL_FALSE, &(matNormal[0][0]) );


                glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );

            glUseProgram( 0 );
            glBindVertexArray( 0 );
        }

        void SetProjViewMatrix(glm::mat4 matProjView){
            ProjectionMatrix = matProjView * matModel;
        }

        void CreateTexture(char* filename){

            stbi_set_flip_vertically_on_load(true);

            data = stbi_load(filename, &tex_width, &tex_height, &n, 0);

            glGenTextures(1, &TextureID);
            glBindTexture(GL_TEXTURE_2D, TextureID);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

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
