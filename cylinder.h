#ifndef cylinder_h
#define cylinder_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace std;

const float PI = 3.1415926535897932384626433832795;

class Cylinder {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    unsigned int textureMap;
    float shininess;

    Cylinder(unsigned int tMap, glm::vec3 amb = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        float shiny = 32.0f, int sectors = 36)
    {
        this->textureMap = tMap;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
        this->emissive = glm::vec3(0.0f, 0.0f, 0.0f);
        this->sectorCount = sectors;

        setUpCylinderVertexDataAndConfigureVertexAttribute();
    }

    ~Cylinder()
    {
        glDeleteVertexArrays(1, &cylinderVAO);
        glDeleteBuffers(1, &cylinderVBO);
        glDeleteBuffers(1, &cylinderEBO);
    }

    void draw(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);
        lightingShader.setVec3("material.emissive", this->emissive);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureMap);

        lightingShader.setMat4("model", model);

        glBindVertexArray(cylinderVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int cylinderVAO;
    unsigned int cylinderVBO;
    unsigned int cylinderEBO;
    int sectorCount;
    int indexCount;

    void setUpCylinderVertexDataAndConfigureVertexAttribute()
    {
        vector<float> vertices;
        vector<unsigned int> indices;

        float radius = 0.5f;
        float height = 1.0f;
        float sectorStep = 2 * PI / sectorCount;

        // Generate vertices for top and bottom circles
        for (int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = radius * cos(sectorAngle);
            float z = radius * sin(sectorAngle);

            // Top circle vertex
            vertices.push_back(x);
            vertices.push_back(height / 2);
            vertices.push_back(z);
            // Normal pointing up
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            // Texture coordinates
            vertices.push_back((float)i / sectorCount);
            vertices.push_back(1.0f);

            // Bottom circle vertex
            vertices.push_back(x);
            vertices.push_back(-height / 2);
            vertices.push_back(z);
            // Normal pointing down
            vertices.push_back(0.0f);
            vertices.push_back(-1.0f);
            vertices.push_back(0.0f);
            // Texture coordinates
            vertices.push_back((float)i / sectorCount);
            vertices.push_back(0.0f);
        }

        // Side vertices with outward normals
        for (int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = i * sectorStep;
            float x = radius * cos(sectorAngle);
            float z = radius * sin(sectorAngle);
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            // Top
            vertices.push_back(x);
            vertices.push_back(height / 2);
            vertices.push_back(z);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back((float)i / sectorCount);
            vertices.push_back(1.0f);

            // Bottom
            vertices.push_back(x);
            vertices.push_back(-height / 2);
            vertices.push_back(z);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            vertices.push_back((float)i / sectorCount);
            vertices.push_back(0.0f);
        }

        // Generate indices for side
        int base = (sectorCount + 1) * 2;
        for (int i = 0; i < sectorCount; ++i)
        {
            int current = base + i * 2;
            int next = base + (i + 1) * 2;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(current + 1);
        }

        // Top cap center
        int topCenterIndex = vertices.size() / 8;
        vertices.push_back(0.0f);
        vertices.push_back(height / 2);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        // Bottom cap center
        int bottomCenterIndex = vertices.size() / 8;
        vertices.push_back(0.0f);
        vertices.push_back(-height / 2);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        // Top cap indices
        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(i * 2);
            indices.push_back((i + 1) * 2);
        }

        // Bottom cap indices
        for (int i = 0; i < sectorCount; ++i)
        {
            indices.push_back(bottomCenterIndex);
            indices.push_back((i + 1) * 2 + 1);
            indices.push_back(i * 2 + 1);
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &cylinderVAO);
        glGenBuffers(1, &cylinderVBO);
        glGenBuffers(1, &cylinderEBO);

        glBindVertexArray(cylinderVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif /* cylinder_h */
