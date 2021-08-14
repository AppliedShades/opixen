#ifndef OPIXEN_RENDERER_H
#define OPIXEN_RENDERER_H
#include <opixen/error.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace OPIXEN{
    class RENDERER{
        public:
            RENDERER(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT);
            //~RENDERER();
            std::vector<unsigned int> screen;
            unsigned int width,height,x,y;
            std::string title;
            GLFWwindow* wnd;
            static void ResizeCallback(GLFWwindow* window, int width, int height){}
    };

    RENDERER::RENDERER(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT):width(WIDTH),height(HEIGHT),title(TITLE){
        //screen.reserve(width*height);
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API , GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //Splash screen like - no backgrounds..
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove the border and titlebar.. 
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); //Enables Double buffer..
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE); // Always on top.. Cannot be used with fullscreen..
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //Splash screen like - no backgrounds..
        

        const GLFWvidmode* vidm = glfwGetVideoMode(glfwGetPrimaryMonitor());

        x=(vidm->width-width)/2,
        y=(vidm->height-height)/2;

        wnd = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        
        if (wnd == NULL)
        {
            errors.report(-12,{"Failed to create GLFW window"},ERRORS::FATAL);
            glfwTerminate();
            return;
        }else{
            glfwMakeContextCurrent(wnd);
            glfwSetFramebufferSizeCallback(wnd, ResizeCallback);
            glfwSetWindowPos(wnd, x, y);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                errors.report(-14,{"Failed to initialize GLAD"},ERRORS::FATAL);
                glfwTerminate();
                return;
            }else{
                glEnable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                //FreeConsole(); //Closes the console window
            }
            
        }

    }
};
#endif