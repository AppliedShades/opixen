#include <opixen/opixen.h>

#include <ft2build.h>
#include FT_FREETYPE_H


class MyApp{
    public:
        FT_Library library;
        FT_Face face;
        MyApp(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT):renderer(TITLE,WIDTH,HEIGHT){
            if (FT_Init_FreeType(&library)) {
                std::cerr << "Error: Failed to initialize FreeType library." << std::endl;
                glfwSetWindowShouldClose(renderer.wnd, true);
                closeApp();
            }
            std::cout << "FreeType library initialized successfully!" << std::endl;
            std::string fontfile = "resources/fonts/ITCEDSCR.TTF";

            if (FT_New_Face(library, fontfile.c_str(), 0, &face)) {
                std::cerr << "Error: Could not load font file.\n";
                glfwSetWindowShouldClose(renderer.wnd, true);
                closeApp();
            }

            anim.setduration(4.0f);
            updatereticle();
            nextPos=glm::vec2(renderer.width,0);
            anim.start(currentTime());
            surface surf;
            surf.metadata = "test surf width=100 height=100 width=200 channels=4";
            setparam(surf.metadata, "width", "400");
            std::string test = getparam(surf.metadata, "width");
            std::cout << surf.metadata << std::endl;
            std::cout << test << std::endl;
            //surfaceinstance sinst = surfaceinstance::create(surf);
            //std::cout << tostr(sinst.points[2]) << std::endl;
        }
        ~MyApp(){
            FT_Done_Face(face);
            FT_Done_FreeType(library);
        }
        void Update();
        void Draw();
        void closeApp(){
            glfwSetWindowShouldClose(renderer.wnd, true);
            AppRunning = false;
        }
        void inline EndFrame(){renderer.EndFrame();}
        void run(){AppRunning=true; while(AppRunning){Update();Draw();EndFrame();}}

        //* Define User Generated functions here:
        ////////////////////////////////////////////*
        void drawcircle(glm::vec2 pos, float radius, glm::vec4 color){
            float diameter = radius*2.0f;
            float minx = pos.x-radius, maxx = pos.x+radius, miny = pos.y-radius, maxy = pos.y+radius;
            minx = minx<0?0:minx, maxx = maxx>renderer.width?renderer.width:maxx, miny = miny<0?0:miny, maxy = maxy>renderer.height?renderer.height:maxy;
            
            for(int j=miny;j<maxy;j++){
                for(int i=minx;i<maxx;i++){
                    float sq_A_plus_sq_B = (pos.x-i)*(pos.x-i) + (pos.y-j)*(pos.y-j);
                    if(sq_A_plus_sq_B < radius*radius){
                        renderer.SetColor(glm::vec2(i,j),color);
                    }
                }
            }
        }
        void drawrect(glm::vec2 pos, glm::vec2 size, glm::vec2 align, glm::vec4 color){
            float minx = pos.x - (size.x*align.x), maxx = pos.x + (size.x*(1.0f-align.x)), miny = pos.y - (size.y*align.y), maxy = pos.y + (size.y*(1.0f-align.y));
            minx = minx<0?0:minx, maxx = maxx>renderer.width?renderer.width:maxx, miny = miny<0?0:miny, maxy = maxy>renderer.height?renderer.height:maxy;
            
            for(int j=miny;j<maxy;j++){
                for(int i=minx;i<maxx;i++){
                    renderer.SetColor(glm::vec2(i,j),color);
                }
            }
        }
        void RenderText(const char* text, glm::vec2 startPos, int fontSize, glm::vec4 color) {
            FT_Set_Pixel_Sizes(face, 0, fontSize);
            glm::vec2 pos = startPos;
            for (const char* p = text; *p; ++p) {
                if (FT_Load_Char(face, *p, FT_LOAD_RENDER)) {
                    //std::cerr << "Error: Could not load character " << *p << ".\n";
                    continue;
                }
        
                FT_GlyphSlot g = face->glyph;
                for (int row = 0; row < g->bitmap.rows; ++row) {
                    for (int col = 0; col < g->bitmap.width; ++col) {
                        unsigned char pixelValue = g->bitmap.buffer[row * g->bitmap.width + col];
                        if (pixelValue > 0) {
                            glm::vec2 pixelPos = glm::vec2(pos.x + g->bitmap_left + col, renderer.height - (pos.y - g->bitmap_top + row));
                            if(pixelPos.x >= 0 && pixelPos.x < renderer.width && pixelPos.y >= 0 && pixelPos.y < renderer.height){
                                renderer.SetColor(pixelPos, color);
                            }
                        }
                    }
                }
                pos.x += g->advance.x >> 6; // Move to the next glyph position
            }

        }
        void RenderTextAntiAliased(const char* text, glm::vec2 startPos, int fontSize, glm::vec4 color) {
            FT_Set_Pixel_Sizes(face, 0, fontSize);
            glm::vec2 pos = startPos;
            for (const char* p = text; *p; ++p) {
                if (FT_Load_Char(face, *p, FT_LOAD_RENDER)) {
                    //std::cerr << "Error: Could not load character " << *p << ".\n";
                    continue;
                }
                
                FT_GlyphSlot g = face->glyph;
                FT_Bitmap bitmap;
                FT_Render_Glyph(g, FT_RENDER_MODE_LCD);
                bitmap = g->bitmap;
                
                for (int row = 0; row < bitmap.rows; ++row) {
                    for (int col = 0; col < bitmap.width; ++col) {
                        unsigned char pixelValue = bitmap.buffer[row * bitmap.width + col];
                        if (pixelValue > 0) {
                            glm::vec2 pixelPos = glm::vec2(pos.x + g->bitmap_left + col, renderer.height - (pos.y - g->bitmap_top + row));
                            if(pixelPos.x >= 0 && pixelPos.x < renderer.width && pixelPos.y >= 0 && pixelPos.y < renderer.height){
                                float alpha = pixelValue / 255.0f;
                                renderer.BlendColor(pixelPos, glm::vec4(color.r, color.g, color.b, alpha));
                            }
                        }
                    }
                }
                pos.x += g->advance.x >> 6; // Move to the next glyph position
            }
        }
    public:
        bool AppRunning = false;
        OPIXEN::OPIXEN renderer;
        animation anim; randomness randomer;

        //* Define User Generated variable here, For example:
        float reticleX=100,reticleY=100,radius=100.0f;
        glm::vec4 reticleColor= glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 nextColor= glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 updatec= glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 direction;
        glm::vec2 reticlePos= glm::vec2(1.0f,1.0f);
        glm::vec2 nextPos= glm::vec2(1.0f,1.0f);
        glm::vec2 updatep= glm::vec2(1.0f,1.0f);
        glm::vec2 direction2;
        void updatereticle(){
            reticleColor=updatec, reticlePos=updatep;
            nextColor = appendw(randomer.getrandom3(0.0f,1.0f),1.0f);
            nextPos.x=randomer.getrandom(0.0f,renderer.width);
            nextPos.y=randomer.getrandom(0.0f,renderer.height);
        }
        ////////////////////////////////////////////*
};

void MyApp::Update(){
    
    //* GLFW, GLAD and GLM already included in opixen, so, you can easily call their functions! 
    
    //Input Handling
    if (glfwGetKey(renderer.wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS){ closeApp(); }


    //World Update
    if(anim.getx(currentTime())>=1.0f){updatereticle(); anim.start(currentTime());}
    updatec = lerp(reticleColor, nextColor, anim.getx_cl(currentTime()));
    updatep = lerp(reticlePos, nextPos, anim.getx_cl(currentTime()));
    reticleX= updatep.x+radius;
    reticleY= updatep.y+radius;

}

void MyApp::Draw(){
    renderer.ClearFrame();
    
    drawrect(glm::vec2(renderer.width*0.1f,100.0f),glm::vec2(renderer.width*0.8f,5.0f),glm::vec2(0.0f,0.5f),glm::vec4(0.5f,0.5f,0.5f,1.0f));
    drawcircle(updatep,radius,updatec);
    drawcircle(glm::vec2(anim.getx(currentTime())*renderer.width*0.8f+ renderer.width*0.1f, 100.0f),20.0f,updatec);
    float fontsize = 120.0f;
    RenderTextAntiAliased("Hello World!", glm::vec2(100.0f, 300.0f+fontsize), fontsize, updatec);
}

//Trusty old Main Function:
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow ) {

    MyApp myapp("Opixen",800,600);
    if (OPIXEN::errors.ErrorState() == OPIXEN::ERRORS::FATAL) { OPIXEN::errors.printErrors(); std::cin.get(); return -1; }
    myapp.run();
    return 0;

}