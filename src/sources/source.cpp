#include <opixen/opixen.h>
class MyApp{
    public:
        MyApp(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT);
        ~MyApp();
        void Update();
        void Draw();
        void inline EndFrame(){renderer.EndFrame();}
        void run(){AppRunning=true; while(AppRunning){Update();Draw();EndFrame();}}

        //* Define User Generated functions here:
        ////////////////////////////////////////////*
    public:
        bool AppRunning = false;
        OPIXEN::OPIXEN renderer;

        //* Define User Generated variable here, For example:
        float reticleX=100,reticleY=100,radius=100.0f;
        glm::vec4 reticleColor= glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 nextColor= glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 direction;
        glm::vec2 reticlePos= glm::vec2(1.0f,1.0f);
        glm::vec2 nextPos= glm::vec2(1.0f,1.0f);
        glm::vec2 direction2;
        ////////////////////////////////////////////*
};

MyApp::MyApp(std::string TITLE, unsigned int WIDTH, unsigned int HEIGHT):renderer(TITLE,WIDTH,HEIGHT){
}
MyApp::~MyApp(){}

void MyApp::Update(){
    
    //* GLFW, GLAD and GLM already included in opixen, so, you can easily call their functions! 
    
    //Input Handling
    if (glfwGetKey(renderer.wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(renderer.wnd, true);
        AppRunning = false;
    }


    //World Update
    //We pick some random location and random color in 3d space and chase them like a hungry cheetah
    if(glm::distance(reticleColor,nextColor)< 3.5f){
        reticleColor=nextColor;
        nextColor.x=float(rand()%1020)/4.0f;
        nextColor.y=float(rand()%1020)/4.0f;
        nextColor.z=float(rand()%1020)/4.0f;
        nextColor.w=float(rand()%1020)/4.0f;
    }
    direction = glm::normalize(nextColor-reticleColor);
    reticleColor+= direction * 7.0f;
    
    if(glm::distance(reticlePos,nextPos)< 3.0f){
        reticlePos=nextPos;
        nextPos.x=float(rand()%2400)/4.0f;
        nextPos.y=float(rand()%1600)/4.0f;
    }
    direction2 = glm::normalize(nextPos-reticlePos);
    reticlePos+= direction2 * 3.0f;
    reticleX= reticlePos.x+radius;
    reticleY= reticlePos.y+radius;

}

void MyApp::Draw(){
    renderer.ClearFrame();


    //* The Color Format for SetColor is ABGR, 8 bit per channel, 1 unsigned int per pixel, so we convert our colors to that format
    unsigned int r=reticleColor.x,g=reticleColor.y,b=reticleColor.z,a=reticleColor.w;
    unsigned int reticleUpdatedColor = r + (g << 8) + (b << 16) + (a << 24);

    renderer.SetClearColor(reticleUpdatedColor);


    //This code draws a reticle, If you don't understand how this works go back to A level or highschool and learn vector and pythagoran theorem properly
    for(int j=reticleY-radius;j<reticleY+radius;j++){
        for(int i=reticleX-radius;i<reticleX+radius;i++){
            float kal = (reticleX-i)*(reticleX-i) + (reticleY-j)*(reticleY-j);
            if(kal < radius*radius){
                if(kal < radius*radius/4.0f){
                    renderer.SetColor(glm::vec2(i,j),0xff0000ff);
                }else{
                    renderer.SetColor(glm::vec2(i,j),0x00);
                }
            }
        }
    }
}

//Trusty old Main Function:
int main(){

    MyApp myapp("Opixen",800,600);
    if (OPIXEN::errors.ErrorState() == OPIXEN::ERRORS::FATAL) { OPIXEN::errors.printErrors(); std::cin.get(); return -1; }
    myapp.run();
    return 0;

}