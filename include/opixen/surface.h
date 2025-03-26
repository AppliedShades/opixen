#include <vector>
#include <opixen/strings.h>
#include <opixen/params.h>
#include <glm/glmheaders.h>
class surface{
    public:
        std::vector<float> pixels;
        std::string metadata;
        inline std::string param(std::string param_n){
            return getparam(metadata, param_n);
        }
        
};
class surfaceinstance{
    public:
        std::vector<float> cached_pixels;
        std::string metadata = "";
        surface* parentsurface;
        glm::vec2 points[4];
        void movepoints(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3){}
        void rotate(float angle){}
        void translate(glm::vec2 offset){}
        void scale(glm::vec2 scale){}
        inline std::string param(std::string param_n){
            return getparam(metadata, param_n);
        }
        static surfaceinstance create(surface &surf){
            surfaceinstance instance;
            instance.parentsurface = &surf;
            instance.metadata = surf.metadata;
            instance.cached_pixels = surf.pixels;
            unsigned int w_ = touint(instance.param("width")), h_ = touint(instance.param("height"));
            instance.points[0] = glm::vec2(0,0);
            instance.points[1] = glm::vec2(w_,0);
            instance.points[2] = glm::vec2(w_,h_);
            instance.points[3] = glm::vec2(0,h_);
            return instance;
        }
        void blit(surfaceinstance &to, glm::vec2 offset){
        }

};