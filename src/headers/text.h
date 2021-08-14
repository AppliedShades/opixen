#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H
#include <vector>
#include <map>
#include <string>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <shader.h>
#include <engine.h>
#include <renderer.h>


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRenderer{
    private:
        Shader defaultShader;
        std::map<GLchar, Character> Characters;
        unsigned int VAO, VBO;
        std::vector<std::pair<int,std::string>> errors;
    public:
        float ysub=0,ysuper=0,fully=0;
        TextRenderer(Renderer& rn);
        float GetTextWidth(std::string& text);
        void RenderTextQuad(Engine::quad textquad, glm::vec3 color, std::string text);
        void RenderText(std::string& text, float x, float y, float scale, glm::vec3 color);
        bool hasError(){return errors.size()>0? true:false;}
        void printErrors();
};
TextRenderer::TextRenderer(Renderer& rn):defaultShader("resources\\shaders\\text.vs", "resources\\shaders\\text.fs"){
    int error=0;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(rn.wnd.width), 0.0f, static_cast<float>(rn.wnd.height));
    defaultShader.use();
    glUniformMatrix4fv(glGetUniformLocation(defaultShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    FT_Library ft;
    if (error = FT_Init_FreeType(&ft))
    {
        errors.push_back({error,"ERROR::FREETYPE: Could not init FreeType Library"});
        return;
    }
    std::string font_name = "resources\\fonts/Dosis-Medium.ttf";
    if (error = font_name.empty())
    {
        errors.push_back({error,"ERROR::FREETYPE: Failed to load font_name"});
        return;
    }
    FT_Face face;
    if (error = FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        errors.push_back({error,"ERROR::FREETYPE: Failed to load font"});
        return;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                errors.push_back({-17,"ERROR::FREETYTPE: Failed to load Glyph"});
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int)(face->glyph->advance.x)
            };
            
            float sub = character.Size.y - character.Bearing.y;
            if(ysuper<character.Bearing.y){ysuper=character.Bearing.y;}
            if(ysub<sub){ysub=sub;}
            Characters.insert(std::pair<char, Character>(c, character));
        }
        fully=ysub+ysuper;
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::RenderTextQuad(Engine::quad textquad, glm::vec3 color, std::string text){
    Character ch;
    float sub=0, fullx=0, x=0;
    Engine::quad temp={0},finalq = {0};
    std::string::const_iterator c;
    defaultShader.use();
    glUniform3f(glGetUniformLocation(defaultShader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    // iterate through all characters
    for (c = text.begin(); c != text.end(); c++)
    {
        ch = Characters[*c];
        fullx += ch.Advance >> 6;
    }
    for (c = text.begin(); c != text.end(); c++)
    {
        ch = Characters[*c];
        temp.locations[0] = x + ch.Bearing.x;//x1
        temp.locations[1] = ysub - (ch.Size.y - ch.Bearing.y);//y1
        temp.locations[4] = temp.locations[0]+ch.Size.x;//x2
        temp.locations[5] = ysub + ch.Bearing.y;//y2
        temp.locations[8] = temp.locations[4];//x3
        temp.locations[9] = temp.locations[1];//y3
        temp.locations[12] = temp.locations[0];//x4
        temp.locations[13] = temp.locations[5];//y4
        
        for(int i=0;i<4;i++){
            temp.locations[i*4]/=fullx,
            temp.locations[i*4 +1]/=fully;
        }
        for(int i=0;i<4;i++){
            finalq.locations[i*4]= temp.locations[i*4 +1] * (temp.locations[i*4] * (textquad.locations[0]+textquad.locations[4]-textquad.locations[8]-textquad.locations[12]) + textquad.locations[12]-textquad.locations[0]) + temp.locations[i*4] * (textquad.locations[8]-textquad.locations[0]) + textquad.locations[0];
            finalq.locations[i*4 +1]= temp.locations[i*4 +1] * (temp.locations[i*4] * (textquad.locations[1]+textquad.locations[5]-textquad.locations[9]-textquad.locations[13]) + textquad.locations[13]-textquad.locations[1]) + temp.locations[i*4] * (textquad.locations[9]-textquad.locations[1]) + textquad.locations[1];
        }
        // update VBO for each character
        float vertices[6][4] = {
            { finalq.locations[12],  finalq.locations[13],  0.0f, 0.0f },//(x1,y1)
            { finalq.locations[0], finalq.locations[1] ,0.0f, 1.0f }, //(x4,y4)
            { finalq.locations[8],  finalq.locations[9],  1.0f, 1.0f },//(x2,y2)

            { finalq.locations[12],  finalq.locations[13],  0.0f, 0.0f }, //(x1,y1)
            { finalq.locations[8],  finalq.locations[9],  1.0f, 1.0f }, //(x2,y2)
            { finalq.locations[4],  finalq.locations[5],  1.0f, 0.0f }, //(x3,y3)

        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderText(std::string& text, float x, float y, float scale, glm::vec3 color)
{
    //get width-height of text
    Character ch;
    float xpos, ypos, w, h;
    std::string::const_iterator c;
    // activate corresponding render state	
    defaultShader.use();
    glUniform3f(glGetUniformLocation(defaultShader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    for (c = text.begin(); c != text.end(); c++)
    {
        ch = Characters[*c];

        xpos = x + ch.Bearing.x * scale;
        ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        w = ch.Size.x * scale;
        h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },// (0,1)x4y4
            { xpos,     ypos,       0.0f, 1.0f },// (0,0)x1y1
            { xpos + w, ypos,       1.0f, 1.0f },// (1,0)x3y3

            { xpos,     ypos + h,   0.0f, 0.0f },// (0,1)x4y4
            { xpos + w, ypos,       1.0f, 1.0f },// (1,0)x3y3
            { xpos + w, ypos + h,   1.0f, 0.0f } // (1,1)x2y2
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

float TextRenderer::GetTextWidth(std::string& text) {
    //get width of text
    float twidth = 0;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        twidth += (Characters[*c].Advance >> 6);
    }
    return twidth;
}
void TextRenderer::printErrors(){
    for(int i=0;i<errors.size();i++){
        std::cout << "error " << errors[i].first << ": " << errors[i].second << std::endl;
    }
}
#endif