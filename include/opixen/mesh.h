#ifndef OPIXEN_MESH_H
#define OPIXEN_MESH_H
#include <vector>
#include <glad/glad.h>

namespace OPIXEN{
    
    struct Vertex {
        float position[4] = {0.0f,0.0f,0.0f,1.0f};
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
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float))); //change this to (void*)(8* sizeof(float))); if vertex colors re-instructed
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void drawMesh(Mesh& mesh_in) {
        glBindVertexArray(mesh_in.vao);
        glBindTexture(GL_TEXTURE_2D, mesh_in.texID);
        glDrawElements(GL_TRIANGLES, mesh_in.indices.size(), GL_UNSIGNED_INT, 0);
    }


};

#endif