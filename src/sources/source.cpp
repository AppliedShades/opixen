#include <opixen/renderer.h>
#include <opixen/text.h>


#include <opixen/mesh.h>
#include <opixen/image.h>

namespace OPIXEN{
    class OPIXEN{
        public:
            unsigned int width=800,height=600,vao=0,vbo=0,ebo=0,texid=0;
            std::string title;
            OPIXEN(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT);
            void EndFrame();
            void inline SetColor(glm::vec2 pos, unsigned int color);
            unsigned int inline GetColor(glm::vec2 pos);
            //TextRenderer textrenderer;
            std::vector<unsigned int> screen;
            std::vector<Shader> shaders;
            GLFWwindow* wnd;
            static void ResizeCallback(GLFWwindow* window, int width, int height){}
    };
    void OPIXEN::EndFrame(){
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
    void inline OPIXEN::SetColor(glm::vec2 pos, unsigned int color){
        screen[width * pos.y + pos.x] = color;
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
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove the border and titlebar.. 
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
                //FreeConsole(); //*Closes the console window

                shaders.push_back({ "resources\\shaders\\textured.vs", "resources\\shaders\\textured.fs" });
                glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
                shaders[0].use();
                glUniformMatrix4fv(glGetUniformLocation(shaders[0].ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
                std::cout << WIDTH << " " << HEIGHT << " " << std::endl;
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
}
int main(){
    bool AppRunning = true;
    OPIXEN::OPIXEN renderer("Opixen",1000,400);
    if (OPIXEN::errors.ErrorState() == OPIXEN::ERRORS::FATAL) { OPIXEN::errors.printErrors(); std::cin.get(); return -1; }
    
    while(AppRunning){
        for(int j=20;j<220;j++){
            for(int i=0;i<200;i++){
                renderer.SetColor(glm::vec2(i,j),0xffff0000); //*0xAA BB GG RR
            }
        }
        

        // render
        // ------
        glClearColor(0.00f, 0.00f, 0.0f, 0.7f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.EndFrame();

        if (glfwGetKey(renderer.wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(renderer.wnd, true);
            AppRunning = false;
        }    
    }
    glfwTerminate();
    return 0;

}
/*
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

*/