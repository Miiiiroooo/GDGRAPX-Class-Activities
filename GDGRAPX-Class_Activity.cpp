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