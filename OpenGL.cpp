#include "OpenGL.h"

OpenGL::OpenGL()
{

}

#pragma region Key Callbacks
void OpenGL::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    TranslationInputs(key, action);
    RotationInputs(key, action);
    ScaleInputs(key, action);
    ZoomInputs(key, action);
    AutoRotateInputs(key, action);
}

void OpenGL::TranslationInputs(int key, int action)
{
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_mod += 0.1f;
    }

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_mod -= 0.1f;
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        y_mod += 0.1f;
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        y_mod -= 0.1f;
    }
}

void OpenGL::RotationInputs(int key, int action)
{
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        theta -= 5.f;
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        theta += 5.f;
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        gamma -= 5.f;
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        gamma += 5.f;
    }
}

void OpenGL::ScaleInputs(int key, int action)
{
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        scale -= 0.1f;
    }

    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        scale += 0.1f;
    }
}

void OpenGL::ZoomInputs(int key, int action)
{
    if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        fov -= 1.f;
    }

    if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        fov += 1.f;
    }
}

void OpenGL::AutoRotateInputs(int key, int action)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        right = !right;
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        up = !up;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        forward = !forward;
    }
}
#pragma endregion

#pragma region Setups
bool OpenGL::SetupOpenGL()
{
    if (!glfwInit())
        return false;
        

    window = glfwCreateWindow(width, height, "Aamir Akim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    return true;
}

void OpenGL::SetupShaders()
{
    // Load shader files
    std::string vertS = GetShaderData("Shaders/sample.vert");
    const char* v = vertS.c_str();

    std::string fragS = GetShaderData("Shaders/sample.frag");;
    const char* f = fragS.c_str();


    // Create and compile shaders
    GLuint vertShader = CreateAndCompileShader(GL_VERTEX_SHADER, v);
    GLuint fragShader = CreateAndCompileShader(GL_FRAGMENT_SHADER, f);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
}

void OpenGL::SetupObjectsAndTextures()
{
    // Load 3d object
    LoadObject(attributes, shapes, materials, "3D/myCube.obj");
    for (auto index : shapes[0].mesh.indices)
    {
        mesh_indices.push_back(index.vertex_index);
    }

    // Load textures
    int img_width;
    int img_height;
    int colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes = stbi_load("3D/ayaya.png", &img_width, &img_height, &colorChannels, 0);

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);
    glEnable(GL_DEPTH_TEST);

    // define vertices and indices of triangle
    GLfloat vertices[]
    {
        0.f, 0.5f, 0.f,
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f
    };

    GLuint indices[]
    {
        0, 1, 2
    };


    // Declare buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &EBO);

    // Setup the buffer objects
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GL_FLOAT) * attributes.vertices.size(),
                 &attributes.vertices[0],
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(GL_FLOAT),
                          (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * mesh_indices.size(),
                 mesh_indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * (sizeof(UV) / sizeof(UV[0])),
                 &UV[0],
                 GL_STATIC_DRAW);

    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          2 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#pragma endregion

#pragma region Private Methods
std::string OpenGL::GetShaderData(std::string path)
{
    std::fstream src(path);
    std::stringstream buffer;
    buffer << src.rdbuf();

    return buffer.str();
}

GLuint OpenGL::CreateAndCompileShader(int shaderType, const char* data)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);

    return shader;
}

void OpenGL::LoadObject(tinyobj::attrib_t& attributes, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, std::string path)
{
    std::string warning, error;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());
}
#pragma endregion

void OpenGL::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // camera
        glm::vec3 camPos = glm::vec3(0, 0, 0.f);
        glm::mat4 camPosMatrix = glm::translate(identity_matrix4, -camPos);
        glm::vec3 worldForward = glm::normalize(glm::vec3(0, 0, 1.f));
        glm::vec3 worldUp = glm::normalize(glm::vec3(0, 1.f, 0));
        glm::vec3 worldRight = glm::normalize(glm::vec3(1.f, 0, 0));

        {
            glm::vec3 camCenter = camPos + glm::vec3(0, 0, 1);
            glm::vec3 F = glm::normalize(camCenter - camPos);
            glm::vec3 R = glm::normalize(glm::cross(F, worldUp));
            glm::vec3 U = glm::normalize(glm::cross(R, F));

            glm::mat4 camOrientation = glm::mat4(1.0f);
            camOrientation[0][0] = R.x;
            camOrientation[1][0] = R.y;
            camOrientation[2][0] = R.z;
            camOrientation[0][1] = U.x;
            camOrientation[1][1] = U.y;
            camOrientation[2][1] = U.z;
            camOrientation[0][2] = -F.x;
            camOrientation[1][2] = -F.y;
            camOrientation[2][2] = -F.z;
            glm::mat4 viewMatrix = camOrientation * camPosMatrix;

            glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(0.f, 1.f, 7.f));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(gamma), glm::normalize(glm::vec3(1.f, 0.f, 0.f)));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));


            unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));

            GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(tex0Address, 0);

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void OpenGL::Cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_UV);
    glDeleteBuffers(1, &EBO);
}
