// GDGRAPX-Class_Activity.cpp : This file contains the 'main' function. Program execution begins and ends there.
// https://free3d.com/3d-model/low_poly_tree-816203.html

#define _USE_MATH_DEFINES
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "stb_image.h"
#include "Model3D.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>


#pragma region Global Variables
// models
std::vector<Model3D> modelsList;
const float scale = 0.1f;
const float spawnCooldown = 3.f;
const float spawnDistance = 5.f;

// time
bool isOnCooldown = false;
float timeSinceStart = 0.f;
float oldTimeSinceStart = 0.f;
float deltaTime = 0.f;
float elapsed = 0.f;

// rotation
float theta = 0;
float phi = 0;

// camera
const float camMovementSpeed = 0.001f; 
const float camRotationSpeed = 0.05f;
glm::vec3 camPos = glm::vec3(0.f);
glm::vec3 camForward = glm::vec3(0, 0, 1.f);
glm::vec3 camRight = glm::vec3(1.f, 0, 0);
std::unordered_map<int, bool> heldKeyInputs;

// mouse
const float mouseSensitivity = 10.4f;
glm::vec2 oldCursorPos = glm::vec2(0.f);
glm::vec2 currentCursorPos = glm::vec2(0.f);

// world orientation
const glm::vec3 worldForward = glm::vec3(0, 0, 1.f);
const glm::vec3 worldUp = glm::vec3(0, 1.f, 0);
const glm::vec3 worldRight = glm::vec3(1.f, 0, 0);

// transformation and projection
float fov = 75.f;
const float width = 600;
const float height = 600;
const glm::mat3 identity_matrix3 = glm::mat3(1.0f);
const glm::mat4 identity_matrix4 = glm::mat4(1.0f);
glm::mat4 perspectiveProjection = glm::perspective(glm::radians(fov), height / width, 0.1f, 100.f);
#pragma endregion

#pragma region Method Declarations
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);
void CursorPositionCallback(GLFWwindow* window, double xPos, double yPos);

void TranslationInputs();
void RotationInputs();
void CalculateCameraOrientation();
void SpawnModel();
void CheckSpawnCooldown();

bool InitializeOpenGL(GLFWwindow** window);
std::string GetShaderData(std::string path);
GLuint CreateAndCompileShader(int shaderType, const char* data);
bool LoadObject(tinyobj::attrib_t& attributes, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, std::vector<GLuint>& mesh_indices, std::string path);
glm::mat4 CreateViewMatrix();
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

    if (!LoadObject(attributes, shapes, materials, mesh_indices, "3D/tree.obj"))
        return -1;
    

    // Declare buffer objects
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    SpawnModel();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        TranslationInputs();
        RotationInputs();
        CheckSpawnCooldown(); 
        glm::mat4 viewMatrix = CreateViewMatrix();

        for (Model3D model : modelsList)
        {
            model.Draw(shaderProgram, identity_matrix4);

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view"); 
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)); 

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection"); 
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection)); 

            glm::vec3 color = glm::vec3(1, 0, 0); 
            unsigned int colorShader = glGetUniformLocation(shaderProgram, "newColor"); 
            glUniform3fv(colorShader, 1, glm::value_ptr(color)); 

            glUseProgram(shaderProgram); 
            glBindVertexArray(VAO); 
            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0); 
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();

    return 0;
}


#pragma region Key Callbacks
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    if (action == GLFW_PRESS && (
        key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_D || key == GLFW_KEY_A ||
        key == GLFW_KEY_Q || key == GLFW_KEY_E ||
        key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT))
    {
        heldKeyInputs[key] = true;
    }
    else if (action == GLFW_RELEASE && (
             key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_D || key == GLFW_KEY_A ||
             key == GLFW_KEY_Q || key == GLFW_KEY_E ||
             key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT))
    {
        heldKeyInputs[key] = false;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        SpawnModel();
    }
}

void CursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    oldCursorPos = currentCursorPos;
    currentCursorPos = glm::vec2(xPos, yPos);

    glm::vec2 mouseDir = currentCursorPos - oldCursorPos;
    mouseDir = glm::normalize(mouseDir);

    phi += mouseDir.x * camRotationSpeed * mouseSensitivity;
    if (phi < -360)
    {
        phi += 360;
    }
    else if (phi > 360)
    {
        phi -= 360;
    }

    theta -= mouseDir.y * camRotationSpeed * mouseSensitivity;
    if (theta < -70)
    {
        theta = -70;
    }
    else if (theta > 70)
    {
        theta = 70;
    }

    CalculateCameraOrientation();
}

void TranslationInputs()
{
    if (heldKeyInputs[GLFW_KEY_W])
    {
        camPos += camForward * camMovementSpeed;
    }

    if (heldKeyInputs[GLFW_KEY_S])
    {
        camPos -= camForward * camMovementSpeed;
    }

    if (heldKeyInputs[GLFW_KEY_D])
    {
        camPos += camRight * camMovementSpeed;
    }

    if (heldKeyInputs[GLFW_KEY_A])
    {
        camPos -= camRight * camMovementSpeed;
    }

    if (heldKeyInputs[GLFW_KEY_Q])
    {
        camPos.y += camMovementSpeed;
    }

    if (heldKeyInputs[GLFW_KEY_E])
    {
        camPos.y -= camMovementSpeed;
    }
}

void RotationInputs()
{
    bool hasChanged = false;

    if (heldKeyInputs[GLFW_KEY_UP])
    {
        theta -= 2.f * camRotationSpeed;
        if (theta < -70)
        {
            theta = -70;
        }
        hasChanged = true;
    }

    if (heldKeyInputs[GLFW_KEY_DOWN])
    {
        theta += 2.f * camRotationSpeed;
        if (theta > 70)
        {
            theta = 70;
        }
        hasChanged = true;
    }

    if (heldKeyInputs[GLFW_KEY_LEFT])
    {
        phi -= 2.f * camRotationSpeed;
        if (phi < -360)
        {
            phi += 360;
        }
        hasChanged = true;
    }

    if (heldKeyInputs[GLFW_KEY_RIGHT])
    {
        phi += 2.f * camRotationSpeed;
        if (phi > 360)
        {
            phi -= 360;
        }
        hasChanged = true;
    }

    if (hasChanged)
    {
        CalculateCameraOrientation();
    }
}

void CalculateCameraOrientation()
{
   /* float z = std::cos(phi) * std::cos(theta);
    float x = std::sin(phi) * cos(theta);
    float y = sin(theta);

    camForward = glm::normalize(glm::vec3(x, y, z)); 
    camRight = glm::normalize(glm::vec3(z, y, x));*/

    glm::vec3 temp1 = camForward;
    glm::vec3 temp2 = camRight;

    std::cout << "phi: " << phi << " theta: " << theta << "\ninitial camForward: " << camForward.x << " " << camForward.y << " " << camForward.z << "\n";

    glm::mat4 camDirMatrix = glm::rotate(identity_matrix4, glm::radians(theta), glm::normalize(glm::cross(worldUp, camForward)));
    camDirMatrix = glm::rotate(camDirMatrix, glm::radians(phi), worldUp);
    camForward = camDirMatrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    camRight = camDirMatrix * glm::vec4(1.f, 0.f, 0.f, 1.f);


    glm::mat4 testMatrix = glm::rotate(identity_matrix4, glm::radians(-63.f), glm::normalize(glm::cross(worldUp, temp1)));
    testMatrix = glm::rotate(camDirMatrix, glm::radians(11.f), worldUp);
    temp1 = testMatrix * glm::vec4(0.f, 0.f, 1.f, 1.f);
    temp2 = testMatrix * glm::vec4(1.f, 0.f, 0.f, 1.f);


    // calculate forward vector using pitch and yaw
    // https://stackoverflow.com/questions/1568568/how-to-convert-euler-angles-to-directional-vector
    // https://stackoverflow.com/questions/10569659/camera-pitch-yaw-to-direction-vector

    std::cout << "calculated camForward: " << camForward.x << " " << camForward.y << " " << camForward.z << "\n\n";
    //std::cout << temp1.x << " " << temp1.y << " " << temp1.z << "\n\n";
}

void SpawnModel()
{
    if (!isOnCooldown)
    {
        glm::vec3 position = camPos + (camForward * spawnDistance);

        glm::vec3 inverseCamForward = glm::inverse(glm::translate(identity_matrix4, camForward)) * glm::vec4(0.f, 0.f, 0.f, 1.f);
        float yRot = glm::degrees(std::acos(glm::dot(inverseCamForward, worldForward) / glm::length(inverseCamForward))); 
        yRot *= inverseCamForward.x > 0 ? 1 : -1;
        glm::vec3 rotation = glm::vec3(0.f, yRot, 0.f);

        Model3D model;
        model.SetPosition(position);
        model.SetRotation(rotation);
        model.SetScale(scale);

        modelsList.push_back(model);
        isOnCooldown = true;
    }
}

void CheckSpawnCooldown()
{
    if (isOnCooldown)
    {
        timeSinceStart = glfwGetTime();
        deltaTime = timeSinceStart - oldTimeSinceStart;
        oldTimeSinceStart = timeSinceStart;

        elapsed += deltaTime;

        if (elapsed > spawnCooldown)
        {
            elapsed = 0.f;
            isOnCooldown = false;
        }
    }
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
    glfwSetCursorPosCallback(*window, CursorPositionCallback);
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
    std::string path)
{
    std::string warning, error;

    if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str()))
    {
        for (auto index : shapes[0].mesh.indices)
        {
            mesh_indices.push_back(index.vertex_index);
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
#pragma endregion