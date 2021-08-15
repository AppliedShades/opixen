#ifndef OPIXEN_H
#define OPIXEN_H
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opixen/error.h>
#include <opixen/shader.h>

namespace OPIXEN{
    class OPIXEN{
        public:
            unsigned int width=800,height=600,vao=0,vbo=0,ebo=0,texid=0;
            std::string title;
            OPIXEN(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT);
            ~OPIXEN();
            void EndFrame();
            void ClearFrame(){std::fill(screen.begin(), screen.end(), ClearColor);}
            void inline SetClearColor(unsigned int colorABGR){ClearColor=colorABGR;}
            void inline SetColor(glm::vec2 pos, unsigned int colorABGR);
            unsigned int inline GetColor(glm::vec2 pos);
            std::vector<unsigned int> screen;
            std::vector<Shader> shaders;
            GLFWwindow* wnd;
            unsigned int ClearColor=0;
            static void ResizeCallback(GLFWwindow* window, int width, int height){}
    };
    void OPIXEN::EndFrame(){
        glClear(GL_COLOR_BUFFER_BIT);
        shaders[0].use();
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texid);
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height, GL_RGBA,GL_UNSIGNED_BYTE,(const void*)(&screen[0]));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(wnd);
        glfwPollEvents();
    }
    void inline OPIXEN::SetColor(glm::vec2 pos, unsigned int colorABGR){
        screen[width * pos.y + pos.x] = colorABGR;
    }
    unsigned int inline OPIXEN::GetColor(glm::vec2 pos){
        return screen[width * pos.y + pos.x];
    }
    OPIXEN::OPIXEN(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT):
        
        //*Initialization-start
        width(WIDTH),height(HEIGHT),title(TITLE)//,textrenderer(WIDTH,HEIGHT)
        //*Initialization-end,Constructor-start
    {

        screen.reserve(width*height);
        screen.insert(screen.end(),width*height,0);
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API , GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //Splash screen like - no backgrounds..
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE); // Remove the border and titlebar.. 
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); //Enables Double buffer..
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE); // Always on top.. Cannot be used with fullscreen..
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); //Splash screen like - no backgrounds..
        

        const GLFWvidmode* vidm = glfwGetVideoMode(glfwGetPrimaryMonitor());

        int x=(vidm->width-width)/2,
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
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                //FreeConsole(); //*Closes the console window

                shaders.push_back({ "resources\\shaders\\textured.vs", "resources\\shaders\\textured.fs" });
                glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
                shaders[0].use();
                glUniformMatrix4fv(glGetUniformLocation(shaders[0].ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                float vertices[16] = {
                     0.0f,         0.0f,          0.0f, 0.0f,  // bottom left
                     float(WIDTH), float(HEIGHT), 1.0f, 1.0f,  // top right
                     float(WIDTH), 0.0f,          1.0f, 0.0f,  // bottom right
                     0.0f,         float(HEIGHT), 0.0f, 1.0f}; // top left
                unsigned int indices[6] = {
                    0, 2, 1,
                    0, 1, 3
                };
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);
                glGenBuffers(1,&vbo);
                glGenBuffers(1,&ebo);
                glBindBuffer(GL_ARRAY_BUFFER,vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, (const void*)(&vertices[0]), GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, (const void*)(&indices[0]), GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
                glBindVertexArray(0);
                
                glGenTextures(1, &texid);
                glBindTexture(GL_TEXTURE_2D, texid);
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height, 0,GL_RGBA,GL_UNSIGNED_BYTE,(const void*)(&screen[0]));
                glBindTexture(GL_TEXTURE_2D, 0);
                
            }
            
        }

    }
    OPIXEN::~OPIXEN(){
        glDeleteTextures(1,&texid);
        glDeleteProgram(shaders[0].ID);
        glDeleteBuffers(1,&ebo);
        glDeleteBuffers(1,&vbo);
        glDeleteVertexArrays(1,&vao);
        glfwDestroyWindow(wnd);
        glfwTerminate();
    }
}

#endif