#pragma once

#include "tiny_obj_loader.h"
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

class OpenGL
{
public:
	OpenGL();
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);
	bool SetupOpenGL();
	void SetupShaders();
	void SetupObjects();
	void Run();
	void Cleanup();

private:
	// inputs
	void TranslationInputs(int key, int action);
	void RotationInputs(int key, int action);
	void ScaleInputs(int key, int action);
	void ZoomInputs(int key, int action);
	void AutoRotateInputs(int key, int action);

	//setup
	std::string GetShaderData(std::string path);
	GLuint CreateAndCompileShader(int shaderType, const char* data);
	void LoadObject(tinyobj::attrib_t& attributes, std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, std::string path);



public:
	GLFWwindow* window;

private:
	// shaders
	GLuint shaderProgram;

	// 3d objects
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector<GLuint> mesh_indices;

	// buffer objects
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	// translation and scale
	float x_mod = 0;
	float y_mod = 0;
	float z_mod = 0;
	float scale = 5;

	// rotation
	float theta = 0;
	float gamma = 0;
	float beta = 0;
	bool right = true;
	bool up = true;
	bool forward = true;

	// transformation and projection
	float fov = 60.f;
	float width = 640;
	float height = 640;
	glm::mat3 identity_matrix3 = glm::mat3(1.0f);
	glm::mat4 identity_matrix4 = glm::mat4(1.0f);
	glm::mat4 orthoProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.0f, 1.0f);
	glm::mat4 perspectiveProjection = glm::perspective(glm::radians(fov), height / width, 0.1f, 100.f);
};

