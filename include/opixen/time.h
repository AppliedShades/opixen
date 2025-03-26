#include <glm/glmheaders.h>
#include <chrono>
#include <random>
typedef std::chrono::high_resolution_clock::time_point timepoint;
float lerp(float a, float b, float t){return a + t*(b-a);}
double lerp(double a, double b, double t){return a + t*(b-a);}
glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float t){return a + t*(b-a);}
glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t){return a + t*(b-a);}
glm::vec4 lerp(glm::vec4 a, glm::vec4 b, float t){return a + t*(b-a);}
glm::vec3 discardw(glm::vec4 v){return glm::vec3(v.x,v.y,v.z);}
glm::vec4 appendw(glm::vec3 v, float w){return glm::vec4(v.x,v.y,v.z,w);}
timepoint currentTime(){return std::chrono::high_resolution_clock::now();}
class animation{
    public:    
        void start(timepoint currenttime){starttime=currenttime;}
        double getx(timepoint currenttime){
            auto timepassed = std::chrono::duration_cast<std::chrono::duration<double>>(currenttime-starttime);
            return timepassed.count()/duration.count();
        }
        double getx_cl(timepoint currenttime){
            return std::min(1.0,getx(currenttime)); // Clamped version
        }
        void setduration(double sec){
            duration = std::chrono::duration<double>(sec);
        }
        void setduration_ms(double ms){
            duration = std::chrono::duration<double>(ms/1000.0);
        }
        timepoint endsat() const {
            return starttime + std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        }
        std::chrono::duration<double> duration = std::chrono::duration<double>::zero();
        timepoint starttime;
};
class randomness{
    public:
        std::random_device rd;
        std::mt19937 gen = std::mt19937(123); // fixed seed // std::mt19937(rd()); //random seed
        std::uniform_real_distribution<float> dis = std::uniform_real_distribution<float>(0.0f, 1.0f);
        inline float getrandom(float min = 0.0f, float max = 1.0f) {
            return std::uniform_real_distribution<float>(min, max)(gen);
        }
        
        inline glm::vec2 getrandom2(float min = 0.0f, float max = 1.0f) {
            return glm::vec2(
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen)
            );
        }
        
        inline glm::vec3 getrandom3(float min = 0.0f, float max = 1.0f) {
            return glm::vec3(
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen)
            );
        }
        
        inline glm::vec4 getrandom4(float min = 0.0f, float max = 1.0f) {
            return glm::vec4(
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen), 
                std::uniform_real_distribution<float>(min, max)(gen)
            );
        }
        
};
