// GDGRAPX-Class_Activity.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define _USE_MATH_DEFINES
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>


#pragma region Global Variables
// translation and scale
float x_mod = 0;
float y_mod = 0;
float z_mod = 0;
float scale = 0.05f;

// rotation
float theta = 0;
float gamma = 0;
float beta = 0;

// camera
glm::vec3 camPos = glm::vec3(0.f);
glm::vec3 camForward = glm::vec3(0, 0, 1.f);
glm::vec3 camRight = glm::vec3(1.f, 0, 0);

// time
float timeSinceStart = 0.f;
float oldTimeSinceStart = 0.f;
float deltaTime = 0.f;
float elapsed = 0.f;

// light
glm::vec3 lightPos = glm::vec3(0.f, 4.f, 0.f);
glm::vec3 lightDir = glm::vec3(-1.f, -1.f, 0.f);
glm::vec3 lightColor = glm::vec3(0.9f, 0.9f, 0.9f);
float ambientStr = 0.1f;
glm::vec3 ambientColor = lightColor; 
float specStr = 0.5f;
float specPhong = 16;

// world orientation
const glm::vec3 worldForward = glm::vec3(0, 0, 1.f);
const glm::vec3 worldUp = glm::vec3(0, 1.f, 0);
const glm::vec3 worldRight = glm::vec3(1.f, 0, 0);

// transformation and projection
float fov = 90.f;
float width = 600;
float height = 600;
glm::mat3 identity_matrix3 = glm::mat3(1.0f);
glm::mat4 identity_matrix4 = glm::mat4(1.0f);
glm::mat4 perspectiveProjection = glm::perspective(glm::radians(fov), height / width, 0.1f, 100.f);
#pragma endregion

#pragma region Method Declarations
void TranslationInputs(int key, int action);
void RotationInputs(int key, int action);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);

bool InitializeOpenGL(GLFWwindow** window);
std::string GetShaderData(std::string path);
GLuint CreateAndCompileShader(int shaderType, const char* data);
bool LoadObject(tinyobj::attrib_t& attributes, 
    std::vector<tinyobj::shape_t>& shapes, 
    std::vector<tinyobj::material_t>& materials, 
    std::vector<GLuint>& mesh_indices, 
    std::vector<GLfloat>& fullVertexData, 
    std::string path);
glm::mat4 CreateViewMatrix();
void ComputeVerticesWithShaders(GLuint& shaderProgram, glm::mat4& transformationMatrix, glm::mat4& viewMatrix);
void ComputeFragmentsWithShaders(GLuint& shaderProgram, GLuint& texture);
#pragma endregion


int main(void)
{
    srand(time(0));
    GLFWwindow* window;
    if (!InitializeOpenGL(&window))
        return -1;

    // Load shader files
    std::string vertS = GetShaderData("Shaders/sample.vert");
    const char* v = vertS.c_str();

    std::string fragS = GetShaderData("Shaders/sample.frag");;
    const char* f = fragS.c_str();

    std::string sky_vertS = GetShaderData("Shaders/skybox.vert");
    const char* sky_v = sky_vertS.c_str();

    std::string sky_fragS = GetShaderData("Shaders/skybox.frag");;
    const char* sky_f = sky_fragS.c_str();

    // Create and compile shaders
    GLuint vertShader = CreateAndCompileShader(GL_VERTEX_SHADER, v);
    GLuint fragShader = CreateAndCompileShader(GL_FRAGMENT_SHADER, f);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    GLuint sky_vertShader = CreateAndCompileShader(GL_VERTEX_SHADER, sky_v);
    GLuint sky_fragShader = CreateAndCompileShader(GL_FRAGMENT_SHADER, sky_f);

    GLuint skyboxProgram = glCreateProgram(); 
    glAttachShader(skyboxProgram, sky_vertShader); 
    glAttachShader(skyboxProgram, sky_fragShader); 
    glLinkProgram(skyboxProgram); 


    // Load 3d object
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;

    if (!LoadObject(attributes, shapes, materials, mesh_indices, fullVertexData, "3D/djSword.obj"))
        return -1;

    // Load textures
    int img_width;
    int img_height;
    int colorChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* tex_bytes = stbi_load("3D/vigar.jpg", &img_width, &img_height, &colorChannels, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);
    glEnable(GL_DEPTH_TEST);

    // Skybox
    /*
      7--------6
     /|       /|
    4--------5 |
    | |      | |
    | 3------|-2
    |/       |/
    0--------1
    */
    //Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    std::string facesSkyboxArray[]{
        "Skybox/rainbow_rt.png",
        "Skybox/rainbow_lf.png",
        "Skybox/rainbow_up.png",
        "Skybox/rainbow_dn.png",
        "Skybox/rainbow_ft.png",
        "Skybox/rainbow_bk.png",
    };

    unsigned int skyboxTex;

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int w, h, sky_color_channel;
        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(facesSkyboxArray[i].c_str(), &w, &h, &sky_color_channel, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        stbi_image_free(data);
    }

    stbi_set_flip_vertically_on_load(true);

    // Declare buffer objects
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Setup the buffer objects
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GL_FLOAT) * fullVertexData.size(),
                 fullVertexData.data(),
                 GL_STATIC_DRAW);

    GLintptr normalPtr = 3 * sizeof(float);
    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(GL_FLOAT),
                          (void*)0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(GL_FLOAT),
                          (void*)normalPtr);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(GL_FLOAT),
                          (void*)uvPtr); 
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        timeSinceStart = glfwGetTime();
        deltaTime = timeSinceStart - oldTimeSinceStart;
        oldTimeSinceStart = timeSinceStart;

        glm::mat4 transformationMatrix = glm::translate(identity_matrix4, glm::vec3(0.f, 0.f, 4.f));
        transformationMatrix = glm::rotate(transformationMatrix, glm::radians(theta), glm::normalize(glm::vec3(0.f, 1.f, 0.f)));
        transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale));

        glm::mat4 viewMatrix = CreateViewMatrix();

        ComputeVerticesWithShaders(shaderProgram, transformationMatrix, viewMatrix);
        ComputeFragmentsWithShaders(shaderProgram, texture);

        theta += 40.0f * deltaTime;

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxProgram);
        glm::mat4 sky_view = glm::mat4(1.0f);
        sky_view = glm::mat4(
            glm::mat3(viewMatrix)
        );

        unsigned int skyViewLoc = glGetUniformLocation(skyboxProgram, "view");
        glUniformMatrix4fv(skyViewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        unsigned int skyProjectionLoc = glGetUniformLocation(skyboxProgram, "projection");
        glUniformMatrix4fv(skyProjectionLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
         
        glBindVertexArray(skyboxVAO);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();

    return 0;
}


#pragma region Input Callbacks
void TranslationInputs(int key, int action)
{
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_mod += 1.0f;
        //camPos.x += 0.4f;
    }

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        x_mod -= 1.0f;
        //camPos.x -= 0.4f;
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        z_mod += 1.0f;
        //camPos.z += 0.4f;
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        z_mod -= 1.0f;
        //camPos.z -= 0.4f;
    }
}

void RotationInputs(int key, int action)
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

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    //TranslationInputs(key, action);
    //RotationInputs(key, action);
}
#pragma endregion

#pragma region Other Methods
bool InitializeOpenGL(GLFWwindow** window)
{
    if (!glfwInit())
        return false;

    *window = glfwCreateWindow(width, height, "Aamir Akim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(*window);
    gladLoadGL();
    glViewport(0, 0, width, height);
    glfwSetKeyCallback(*window, KeyCallback);

    return true;
}

std::string GetShaderData(std::string path)
{
    std::fstream src(path);
    std::stringstream buffer;
    buffer << src.rdbuf();

    return buffer.str();
}

GLuint CreateAndCompileShader(int shaderType, const char* data)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);

    return shader;
}

bool LoadObject(tinyobj::attrib_t& attributes,
    std::vector<tinyobj::shape_t>& shapes,
    std::vector<tinyobj::material_t>& materials,
    std::vector<GLuint>& mesh_indices,
    std::vector<GLfloat>& fullVertexData,
    std::string path)
{
    std::string warning, error;

    if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str()))
    {
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
        {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

            mesh_indices.push_back(vData.vertex_index);

            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 0]);
            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]);
            fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]);

            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 0]);
            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 1]);
            fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 2]);

            fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 0]);
            fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 1]);
        }

        return true;
    }
    else
    {
        std::cout << "error loading obj file\n";
        std::cout << "Error: " << error << "\n";
        std::cout << "Warning: " << warning << "\n";
        return false;
    }
}

glm::mat4 CreateViewMatrix()
{
    glm::mat4 camPosMatrix = glm::translate(identity_matrix4, -camPos);
    glm::vec3 F = glm::normalize(camForward);
    glm::vec3 R = glm::normalize(glm::cross(worldUp, F)); 
    glm::vec3 U = glm::normalize(glm::cross(F, R));

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

    return camOrientation * camPosMatrix;
}

void ComputeVerticesWithShaders(GLuint& shaderProgram, glm::mat4& transformationMatrix, glm::mat4& viewMatrix)
{
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformationMatrix));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
}

void ComputeFragmentsWithShaders(GLuint& shaderProgram, GLuint& texture)
{
    GLuint tex0Address = glGetUniformLocation(shaderProgram, "tex0");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(tex0Address, 0);

    GLuint lightAddress = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));
    GLuint lightColorAddress = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

    GLuint ambientStrAddress = glGetUniformLocation(shaderProgram, "ambientStr");
    glUniform1f(ambientStrAddress, ambientStr);
    GLuint ambientColorAddress = glGetUniformLocation(shaderProgram, "ambientColor");
    glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));

    GLuint camPosAddress = glGetUniformLocation(shaderProgram, "camPos");
    glUniform3fv(camPosAddress, 1, glm::value_ptr(camPos));
    GLuint specStrAddress = glGetUniformLocation(shaderProgram, "specStr");
    glUniform1f(specStrAddress, specStr);
    GLuint specPhongAddress = glGetUniformLocation(shaderProgram, "specPhong");
    glUniform1f(specPhongAddress, specPhong);

    GLuint lightDirAddress = glGetUniformLocation(shaderProgram, "lightDir");
    glUniform3fv(lightDirAddress, 1, glm::value_ptr(lightDir));
}
#pragma endregion