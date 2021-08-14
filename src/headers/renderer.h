#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct WindowProperties{
    int x=0,y=0,width=0,height=0;
    std::string title;
    GLFWwindow* handle = NULL;
};
struct MonitorProperties{
    int width=0,height=0;
    int workAreaX=0,workAreaY=0,workAreaWidth=0,workAreaHeight=0;
    GLFWmonitor* handle;
};
class Renderer{
    public:
        WindowProperties wnd;
        MonitorProperties monitor;
        Renderer(int width,int height, std::string);
        bool hasError(){return errors.size()>0? true:false;}
        void printErrors();
    private:
        static void ResizeCallback(GLFWwindow* window, int width, int height){}
        std::vector<std::pair<int,std::string>> errors;
};
Renderer::Renderer(int width,int height, std::string title){
    
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API , GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove the border and titlebar.. 
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); //Enables Double buffer..
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE); // Always on top.. Cannot be used with fullscreen..
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //Splash screen like - no backgrounds..
    

    monitor.handle = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(monitor.handle,&monitor.workAreaX,&monitor.workAreaY,&monitor.workAreaWidth,&monitor.workAreaHeight);
    {
        const GLFWvidmode* glvidm = glfwGetVideoMode(monitor.handle);
        monitor.width=glvidm->width, monitor.height=glvidm->height;
    }

    wnd.width=width,
    wnd.height=height,
    wnd.x=(monitor.width-wnd.width)/2,
    wnd.y=(monitor.height-wnd.height)/2;
    wnd.handle = glfwCreateWindow(wnd.width, wnd.height, wnd.title.c_str(), NULL, NULL);
    
    if (wnd.handle == NULL)
    {
        errors.push_back({-12,"Failed to create GLFW window"});
        glfwTerminate();
        return;
    }else{
        glfwMakeContextCurrent(wnd.handle);
        glfwSetFramebufferSizeCallback(wnd.handle, ResizeCallback);
        glfwSetWindowPos(wnd.handle, wnd.x, wnd.y);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            errors.push_back({-14,"Failed to initialize GLAD"});
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

void Renderer::printErrors(){
    for(int i=0;i<errors.size();i++){
        std::cout << "error " << errors[i].first << ": " << errors[i].second << std::endl;
    }
}
#endif