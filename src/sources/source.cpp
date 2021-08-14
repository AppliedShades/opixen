#define _USE_MATH_DEFINES

//#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
//#include <imports.h>
#include <renderer.h>
#include <text.h>
#include "stb_image.h"

struct Vertex {
    float position[4] = {0.0f,0.0f,0.0f,1.0f};
    //float color[4] = { 1.0f,1.0f,1.0f,1.0f };
    float texCoord[2];
};
struct Mesh {
    unsigned int vao = 0, vbo = 0, ebo = 0, texID=0;
    std::vector<unsigned int> indices;
    std::vector<Vertex> vertices;
    Mesh(std::vector<Vertex> ver_in, std::vector<unsigned int> ind_in) { 
        vertices.reserve(ver_in.size());
        vertices.insert(vertices.end(), ver_in.begin(), ver_in.end());
        indices.reserve(ind_in.size());
        indices.insert(indices.end(), ind_in.begin(), ind_in.end());
        glGenVertexArrays(1, &vao);
    };
    Mesh(std::vector<Vertex>& ver_in, std::vector<unsigned int>& ind_in) {
        vertices.reserve(ver_in.size());
        vertices.insert(vertices.end(), ver_in.begin(), ver_in.end());
        indices.reserve(ind_in.size());
        indices.insert(indices.end(), ind_in.begin(), ind_in.end());
        glGenVertexArrays(1, &vao);
    }
};
void initMesh(Mesh& mesh_in);
void drawMesh(Mesh& mesh_in);

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

int main()
{
    int errors = 0;
    Renderer openglRenderer(1000,200,"BrickBreaker!:D");
    if (openglRenderer.hasError()) { openglRenderer.printErrors(); return -1; }

    std::vector<Shader> shaders;
    shaders.reserve(2);
    shaders.push_back({ "resources\\shaders\\simple.vs", "resources\\shaders\\simple.fs" });
    shaders.push_back({ "resources\\shaders\\textured.vs", "resources\\shaders\\textured.fs" });
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(openglRenderer.wnd.width), 0.0f, static_cast<float>(openglRenderer.wnd.height));
    for (int i = 0;i < shaders.size();i++) {
        shaders[i].use();
        glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }
    TextRenderer txtRenderer(openglRenderer);
    if (txtRenderer.hasError()) { txtRenderer.printErrors(); return -1; }

    stbi_set_flip_vertically_on_load(true);

    std::vector<std::pair<std::string, ImageData>> imgStorage;
    int iw, ih, nrChannels;
    unsigned char* data;

    std::vector<Mesh> meshes;
    meshes.push_back({ {
        { 200.0f,    200.0f, 0.0f, 1.0f,   1.0f, 1.0f},  // top right
        { 200.0f,    000.0f, 0.0f, 1.0f,   1.0f, 0.0f},  // bottom right
        { 000.0f,    000.0f, 0.0f, 1.0f,   0.0f, 0.0f},  // bottom left
        { 000.0f,    200.0f, 0.0f, 1.0f,   0.0f, 1.0f}   // top left 
        }, {
        3, 1, 0,
        3, 2, 1
        } });
    meshes.push_back({ {
        { 400.0f,  200.0f, 0.0f, 1.0f,  1.0f, 1.0f},  // top right
        { 400.0f,  000.0f, 0.0f, 1.0f,  1.0f, 0.0f},  // bottom right
        { 200.0f,  000.0f, 0.0f, 1.0f,  0.0f, 0.0f},  // bottom left
        { 200.0f,  200.0f, 0.0f, 1.0f,  0.0f, 1.0f}   // top left 
        }, {
        3, 1, 0,
        3, 2, 1
        } });

    for (size_t i = 0; i < meshes.size(); i++) {
        initMesh(meshes[i]);
    }
    
    ImageLocation img = openImageFile("resources\\images\\b.png", imgStorage);
    if (img >= imgStorage.size()) { std::cout << "fatal error: Image loading failed." << std::endl; return 0; }
    meshes[0].texID = genTexFromImage(imgStorage[img].second);

    img = openImageFile("resources\\images\\b.png", imgStorage);
    if (img >= imgStorage.size()) { std::cout << "fatal error: Image loading failed." << std::endl; return 0; }
    meshes[1].texID = genTexFromImage(imgStorage[img].second);


    // render loop
    // -----------
    unsigned int frames = 0;
    //double prevSec = floor(glfwGetTime());
    double prevTime = glfwGetTime();
    double dt = prevTime;
    std::string mils;
    float tw, uiScale=openglRenderer.wnd.width/800.0f,textScale=uiScale;
    bool running=true;
    while (running)
    {

        // input
        // -----
        if (glfwGetKey(openglRenderer.wnd.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(openglRenderer.wnd.handle, true);
            running = false;
        }
        
        
        
        dt = glfwGetTime() - prevTime;
        prevTime = glfwGetTime();
        mils = std::to_string(dt * 1000.0) + " ms/f";

        // render
        // ------
        glClearColor(0.00f, 0.00f, 0.0f, 0.7f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(openglRenderer.wnd.handle, GLFW_KEY_X) == GLFW_PRESS || glfwGetKey(openglRenderer.wnd.handle, GLFW_KEY_S) == GLFW_PRESS) {
            shaders[0].use();
        }
        else {
            shaders[1].use();
        }

        for (size_t i = 0; i < meshes.size(); i++) {
            drawMesh(meshes[i]);
        }
        std::string score = std::to_string(200.0f);
        tw = txtRenderer.GetTextWidth(score);
        textScale = uiScale *0.5f;
        Engine::quad textBox = Engine::makeQuad(openglRenderer.wnd.width- tw*textScale,openglRenderer.wnd.height- txtRenderer.fully*textScale,openglRenderer.wnd.width,openglRenderer.wnd.height,openglRenderer.wnd.width,openglRenderer.wnd.height- txtRenderer.fully*textScale,openglRenderer.wnd.width- tw*textScale,openglRenderer.wnd.height);
        //RenderText(shaders[0], score, SCR_WIDTH-tw* textScale, SCR_HEIGHT - 24.0f * textScale, textScale * 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        //RenderText(shaders[0], mils, 0.0f, 0.0f,textScale * 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
        txtRenderer.RenderTextQuad(textBox,glm::vec3(1.0f,1.0f,1.0f),score);
        tw = txtRenderer.GetTextWidth(mils);
        textBox = Engine::makeQuad(0,0,tw*textScale,txtRenderer.fully*textScale,tw*textScale,0,0,txtRenderer.fully*textScale);
        txtRenderer.RenderTextQuad(textBox,glm::vec3(1.0f,1.0f,1.0f),mils);
        glfwSwapBuffers(openglRenderer.wnd.handle);
        glfwPollEvents();
    }

    for (size_t i = 0; i < imgStorage.size();i++) {
        stbi_image_free(imgStorage[i].second.data);
    }
    glfwTerminate();
    return 0;
}



void initMesh(Mesh& mesh_in) {
    glBindVertexArray(mesh_in.vao);
    glDeleteBuffers(1, &mesh_in.vbo);
    glDeleteBuffers(1, &mesh_in.ebo);
    glGenBuffers(1, &mesh_in.vbo);
    glGenBuffers(1, &mesh_in.ebo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_in.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh_in.vertices.size(), (const void*)(&mesh_in.vertices[0]), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_in.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh_in.indices.size(), (const void*)(&mesh_in.indices[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float))); //change this to (void*)(8* sizeof(float))); if vertex colors re-instructed
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void drawMesh(Mesh& mesh_in) {
    glBindVertexArray(mesh_in.vao);
    glBindTexture(GL_TEXTURE_2D, mesh_in.texID);
    glDrawElements(GL_TRIANGLES, mesh_in.indices.size(), GL_UNSIGNED_INT, 0);
}
