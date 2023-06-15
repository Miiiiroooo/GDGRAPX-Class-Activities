#include "Model3D.h"

Model3D::Model3D()
{
	position = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1);
}

void Model3D::Draw(const GLuint& shaderProgram, const glm::mat4& identity_matrix4)
{
	glm::mat4 transformation_matrix = glm::translate(identity_matrix4, position);
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
	transformation_matrix = glm::rotate(transformation_matrix, glm::radians(rotation.x), glm::vec3(0.f, 0.f, 1.f));
	transformation_matrix = glm::scale(transformation_matrix, scale);

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));
}

void Model3D::SetPosition(const glm::vec3& position)
{
	this->position = position;
}

void Model3D::SetRotation(const glm::vec3& rotation)
{
	this->rotation = rotation;
}

void Model3D::SetScale(const float& scale)
{
	this->scale = glm::vec3(scale);
}

glm::vec3 Model3D::GetPosition()
{
	return position;
}

glm::vec3 Model3D::GetRotation()
{
	return rotation;
}

glm::vec3 Model3D::GetScale()
{
	return scale;
}
