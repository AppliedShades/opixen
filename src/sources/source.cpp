#include <opixen/renderer.h>
#include <opixen/text.h>


#include <opixen/mesh.h>
#include <opixen/image.h>
int main()
{
    int errors = 0;
    OPIXEN::RENDERER openglRenderer("Opixen",1000,200);
    if (OPIXEN::errors.ErrorState() == OPIXEN::ERRORS::FATAL) { OPIXEN::errors.printErrors(); std::cin.get(); return -1; }

    std::vector<Shader> shaders;
    shaders.push_back({ "resources\\shaders\\textured.vs", "resources\\shaders\\textured.fs" });
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(openglRenderer.width), 0.0f, static_cast<float>(openglRenderer.height));
    for (int i = 0;i < shaders.size();i++) {
        shaders[i].use();
        glUniformMatrix4fv(glGetUniformLocation(shaders[i].ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }
    OPIXEN::TextRenderer txtRenderer(openglRenderer);
    if (OPIXEN::errors.ErrorState() == OPIXEN::ERRORS::FATAL) { OPIXEN::errors.printErrors(); std::cin.get(); return -1; }

    stbi_set_flip_vertically_on_load(true);

    std::vector<std::pair<std::string, OPIXEN::ImageData>> imgStorage;
    int iw, ih, nrChannels;
    unsigned char* data;

    std::vector<OPIXEN::Mesh> meshes;
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
    
    OPIXEN::ImageLocation img = OPIXEN::openImageFile("resources\\images\\b.png", imgStorage);
    if (img >= imgStorage.size()) { std::cout << "fatal error: Image loading failed." << std::endl; return 0; }
    meshes[0].texID = OPIXEN::genTexFromImage(imgStorage[img].second);

    img = OPIXEN::openImageFile("resources\\images\\b.png", imgStorage);
    if (img >= imgStorage.size()) { std::cout << "fatal error: Image loading failed." << std::endl; return 0; }
    meshes[1].texID = OPIXEN::genTexFromImage(imgStorage[img].second);


    // render loop
    // -----------
    unsigned int frames = 0;
    //double prevSec = floor(glfwGetTime());
    double prevTime = glfwGetTime();
    double dt = prevTime;
    std::string mils;
    float tw, uiScale=openglRenderer.width/800.0f,textScale=uiScale;
    bool running=true;
    while (running)
    {

        // input
        // -----
        if (glfwGetKey(openglRenderer.wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(openglRenderer.wnd, true);
            running = false;
        }
        
        
        
        dt = glfwGetTime() - prevTime;
        prevTime = glfwGetTime();
        mils = std::to_string(dt * 1000.0) + " ms/f";

        // render
        // ------
        glClearColor(0.00f, 0.00f, 0.0f, 0.7f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaders[0].use();

        for (size_t i = 0; i < meshes.size(); i++) {
            drawMesh(meshes[i]);
        }
        std::string score = std::to_string(200.0f);
        tw = txtRenderer.GetTextWidth(score);
        textScale = uiScale *0.5f;
        
        OPIXEN::quad textBox = OPIXEN::makeQuad(openglRenderer.width- tw*textScale,openglRenderer.height- txtRenderer.fully*textScale,openglRenderer.width,openglRenderer.height,openglRenderer.width,openglRenderer.height- txtRenderer.fully*textScale,openglRenderer.width- tw*textScale,openglRenderer.height);
        txtRenderer.RenderTextQuad(textBox,glm::vec3(1.0f,1.0f,1.0f),score);
        tw = txtRenderer.GetTextWidth(mils);
        
        textBox = OPIXEN::makeQuad(0,0,tw*textScale,txtRenderer.fully*textScale,tw*textScale,0,0,txtRenderer.fully*textScale);
        txtRenderer.RenderTextQuad(textBox,glm::vec3(1.0f,1.0f,1.0f),mils);
        glfwSwapBuffers(openglRenderer.wnd);
        glfwPollEvents();
    }

    for (size_t i = 0; i < imgStorage.size();i++) {
        stbi_image_free(imgStorage[i].second.data);
    }
    glfwTerminate();
    return 0;
}

