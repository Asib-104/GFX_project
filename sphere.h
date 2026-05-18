#ifndef sphere_h
#define sphere_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Sphere {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    glm::vec3 emissive;
    unsigned int textureMap;

    Sphere(unsigned int tMap = 0,
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

        setUpSphereVertexDataAndConfigureVertexAttribute();
    }

    ~Sphere()
    {
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);
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

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int sphereVAO;
    unsigned int sphereVBO;
    unsigned int sphereEBO;
    int sectorCount;
    int stackCount;
    int indexCount;

    void setUpSphereVertexDataAndConfigureVertexAttribute()
    {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float radius = 1.0f;
        float x, y, z, xy;
        float nx, ny, nz;
        float s, t;

        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;

        // Generate vertices
        for (int i = 0; i <= stackCount; ++i)
        {
            stackAngle = M_PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                // Position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normal
                nx = x / radius;
                ny = y / radius;
                nz = z / radius;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                // Texture coordinates
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }

        // Generate indices
        unsigned int k1, k2;
        for (int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

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

        indexCount = indices.size();

        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);

        glBindVertexArray(sphereVAO);

        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
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

#endif /* sphere_h */
