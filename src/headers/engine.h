#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine{
    struct quad{
        float locations[16];
    };
    struct vertcolor{
        float rgba[4];
    };
    quad makeQuad(float x1, float y1,float x2, float y2,float x3, float y3,float x4, float y4){
        return {x1,y1,0.0f,0.0f,x2,y2,0.0f,0.0f,x3,y3,0.0f,0.0f,x4,y4,0.0f,0.0f};
    }
    void logQuad(quad& q){
        std::cout << q.locations[0] << ","<< q.locations[1] << "\t\t" << q.locations[4] << ","<< q.locations[5] << std::endl
        << q.locations[8] << ","<< q.locations[9] << "\t\t" << q.locations[12] << ","<< q.locations[13] << std::endl;
    }
};

#endif