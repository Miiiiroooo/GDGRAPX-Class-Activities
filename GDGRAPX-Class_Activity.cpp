// GDGRAPX-Class_Activity.cpp : This file contains the 'main' function. Program execution begins and ends there.

#define _USE_MATH_DEFINES
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>


float DegToRad(float degrees)
{
    return (degrees * (M_PI / 180));
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    window = glfwCreateWindow(640, 640, "Aamir Akim", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // pentagon
        glBegin(GL_POLYGON);
        for (int i = 0; i < 5; i++)
        {
            glVertex2f(cos(DegToRad(i * 72)) / 2.f, sin(DegToRad(i * 72)) / 2.f);
        }
        glEnd();


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
