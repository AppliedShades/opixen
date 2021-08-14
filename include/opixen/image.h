#ifndef OPIXEN_IMAGE_H
#define OPIXEN_IMAGE_H
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
//#define _USE_MATH_DEFINES
//#define STB_IMAGE_IMPLEMENTATION
#include <vector>
#include <string>
#include <glad/glad.h>
//#include <stb_image/stb_image.h>

namespace OPIXEN{
    struct ImageData {
        int w, h, channels, loaded = 0;
        unsigned char* data;
    };
    typedef size_t ImageLocation;
    ImageLocation openImageFile(std::string fileName,std::vector<std::pair<std::string,ImageData>>& imageStorage) {
        ImageLocation i;
        for (i = 0;i < imageStorage.size();i++) {
            if (fileName.compare(imageStorage[i].first) == 0) { return i; }
        }
        std::pair<std::string, ImageData> x;
        x.first = fileName;
        x.second.data = stbi_load(fileName.c_str(), &x.second.w, &x.second.h, &x.second.channels, 0);
        if (x.second.data) { imageStorage.push_back(x); }
        return i; //if error occured return will be equal to imageStorage size
    }
    unsigned int genTexFromImage(ImageData& img) {
        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + img.channels - 3, img.w, img.h, 0, GL_RGB + img.channels - 3, GL_UNSIGNED_BYTE, img.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        return id;
    }
};

#endif