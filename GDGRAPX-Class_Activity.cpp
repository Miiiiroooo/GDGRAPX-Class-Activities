//#define _USE_MATH_DEFINES
//#define TINYOBJLOADER_IMPLEMENTATION
//
//#include "OpenGL.h"
//
//
//OpenGL* opengl = new OpenGL();
//
//void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
//{
//    opengl->KeyCallback(window, key, scancode, action, mod);
//}
//
//int main(void)
//{
//    if (!opengl->SetupOpenGL())
//        return -1;
//
//    glfwSetKeyCallback(opengl->window, KeyCallback);
//    opengl->SetupShaders();
//    opengl->SetupObjects();
//    opengl->Run();
//    opengl->Cleanup();
//    glfwTerminate();
//
//    return 0;
//}



// save
/*
        {
            glm::vec3 color = glm::vec3(1, 0, 0);
            glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(-1.5f, 1.5f, 0.f));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.f), glm::normalize(glm::vec3(1, 0, 0)));


            unsigned int colorShader = glGetUniformLocation(shaderProgram, "newColor");
            glUniform3fv(colorShader, 1, glm::value_ptr(color));

            unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }

        {
            glm::vec3 color = glm::vec3(0, 1, 0);
            glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(1.5f, 1.5f, 0.f));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(90.f), glm::normalize(glm::vec3(0, 1, 0)));


            unsigned int colorShader = glGetUniformLocation(shaderProgram, "newColor");
            glUniform3fv(colorShader, 1, glm::value_ptr(color));

            unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }

        {
            glm::vec3 color = glm::vec3(0, 0, 1);
            glm::mat4 transformation_matrix = glm::translate(identity_matrix4, glm::vec3(0.f, -1.5f, 0.f));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(180.f), glm::normalize(glm::vec3(0, 1, 0)));


            unsigned int colorShader = glGetUniformLocation(shaderProgram, "newColor");
            glUniform3fv(colorShader, 1, glm::value_ptr(color));

            unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

            unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }
        */