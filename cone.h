#ifndef cone_h
#define cone_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Cone {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    glm::vec3 emissive;
    unsigned int textureMap;

    Cone(unsigned int tMap = 0,
        glm::vec3 amb = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        float shiny = 32.0f,
        int sectors = 36,
        int stacks = 18)
    {
        this->textureMap = tMap;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
        this->emissive = glm::vec3(0.0f, 0.0f, 0.0f);
        this->sectorCount = sectors;
        this->stackCount = stacks;

        setUpConeVertexDataAndConfigureVertexAttribute();
    }

    ~Cone()
    {
        glDeleteVertexArrays(1, &coneVAO);
        glDeleteBuffers(1, &coneVBO);
        glDeleteBuffers(1, &coneEBO);
    }

    void draw(Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f))
    {
        lightingShader.use();

        lightingShader.setInt("texUnit", 0);
        lightingShader.setVec3("material.ambient", this->ambient);
        lightingShader.setVec3("material.diffuse", this->diffuse);
        lightingShader.setVec3("material.specular", this->specular);
        lightingShader.setFloat("material.shininess", this->shininess);
        lightingShader.setVec3("material.emissive", this->emissive);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureMap);

        lightingShader.setMat4("model", model);

        glBindVertexArray(coneVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int coneVAO;
    unsigned int coneVBO;
    unsigned int coneEBO;
    int sectorCount;
    int stackCount;
    int indexCount;

    void setUpConeVertexDataAndConfigureVertexAttribute()
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float baseRadius = 1.0f;
        float height = 2.0f;
        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = height / stackCount;

        // Generate vertices for the cone surface
        for (int i = 0; i <= stackCount; ++i)
        {
            float y = -height / 2.0f + i * stackStep;
            float radius = baseRadius * (1.0f - (float)i / stackCount);
            
            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;
                float x = radius * cosf(sectorAngle);
                float z = radius * sinf(sectorAngle);

                // Position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normal (approximate for cone)
                glm::vec3 pos(x, y, z);
                float len = sqrtf(x * x + z * z);
                glm::vec3 normal;
                if (len > 0.001f) {
                    normal = glm::normalize(glm::vec3(x * height / baseRadius, baseRadius, z * height / baseRadius));
                } else {
                    normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                // Texture coordinates
                float s = (float)j / sectorCount;
                float t = (float)i / stackCount;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }

        // Generate indices for the cone surface
        for (int i = 0; i < stackCount; ++i)
        {
            int k1 = i * (sectorCount + 1);
            int k2 = k1 + sectorCount + 1;

            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        // Add base circle
        int baseCenter = vertices.size() / 8;
        vertices.push_back(0.0f);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(-1.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.5f);

        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = baseRadius * cosf(sectorAngle);
            float z = baseRadius * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(-height / 2.0f);
            vertices.push_back(z);
            vertices.push_back(0.0f);
            vertices.push_back(-1.0f);
            vertices.push_back(0.0f);
            vertices.push_back((cosf(sectorAngle) + 1.0f) * 0.5f);
            vertices.push_back((sinf(sectorAngle) + 1.0f) * 0.5f);
        }

        for (int j = 0; j < sectorCount; ++j)
        {
            indices.push_back(baseCenter);
            indices.push_back(baseCenter + j + 1);
            indices.push_back(baseCenter + j + 2);
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &coneVAO);
        glGenBuffers(1, &coneVBO);
        glGenBuffers(1, &coneEBO);

        glBindVertexArray(coneVAO);

        glBindBuffer(GL_ARRAY_BUFFER, coneVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, coneEBO);
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

#endif /* cone_h */
