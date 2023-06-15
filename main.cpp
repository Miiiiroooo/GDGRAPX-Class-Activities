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


#pragma region Global Variables
// translation and scale
float x_mod = 0;
float y_mod = 0;
float z_mod = 0;
float scale = 1.2f;

// rotation
float theta = 0;
float gamma = 0;
float beta = 0;
bool right = true;
bool up = true;
bool forward = true;

// transformation and projection
float fov = 75.f;
float width = 600;
float height = 600;
glm::mat3 identity_matrix3 = glm::mat3(1.0f);
glm::mat4 identity_matrix4 = glm::mat4(1.0f);
//glm::mat4 orthoProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);
glm::mat4 perspectiveProjection = glm::perspective(glm::radians(fov), height / width, 0.1f, 100.f);
#pragma endregion

#pragma region Key Callbacks
void TranslationInputs(int key, int action)
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

void ScaleInputs(int key, int action)
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

void ZoomInputs(int key, int action)
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

//void AutoRotateInputs(int key, int action)
//{
//    if (key == GLFW_KEY_R && action == GLFW_PRESS)
//    {
//        right = !right;
//    }
//
//    if (key == GLFW_KEY_T && action == GLFW_PRESS)
//    {
//        up = !up;
//    }
//
//    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
//    {
//        forward = !forward;
//    }
//}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    //TranslationInputs(key, action);
    RotationInputs(key, action);
    //ScaleInputs(key, action);
    //ZoomInputs(key, action);
    //AutoRotateInputs(key, action);
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

void LoadObject(tinyobj::attrib_t& attributes, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, std::string path)
{
    std::string warning, error;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());
}
#pragma endregion


int main(void)
{
    GLFWwindow* window;
    if (!InitializeOpenGL(&window))
        return -1;

    // Load shader files
    std::string vertS = GetShaderData("Shaders/sample.vert");
    const char* v = vertS.c_str();

    std::string fragS = GetShaderData("Shaders/sample.frag");;
    const char* f = fragS.c_str();

    // Create and compile shaders
    GLuint vertShader = CreateAndCompileShader(GL_VERTEX_SHADER, v);
    GLuint fragShader = CreateAndCompileShader(GL_FRAGMENT_SHADER, f);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);


    // Load 3d object
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<GLuint> mesh_indices;
    std::vector<GLfloat> fullVertexData;

    LoadObject(attributes, shapes, materials, "3D/myCube.obj");
    for (auto index : shapes[0].mesh.indices)
    {
        mesh_indices.push_back(index.vertex_index);
    }

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++)
    {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 0]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]);

        fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 0]);
        fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 1]);
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

    // Declare buffer objects
    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    /*glGenBuffers(1, &VBO_UV);
    glGenBuffers(1, &EBO);*/

    // Setup the buffer objects
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GL_FLOAT) * fullVertexData.size(),
                 fullVertexData.data(),
                 GL_STATIC_DRAW);


    GLintptr uvPtr = 3 * sizeof(float); 
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(GL_FLOAT),
                          (void*)0);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(GL_FLOAT),
                          (void*)uvPtr); 
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);

    /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
            glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 5);
            //glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &VBO_UV);
    //glDeleteBuffers(1, &EBO);
    glfwTerminate();

    return 0;
}