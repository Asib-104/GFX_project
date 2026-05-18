#ifndef bezier_surface_h
#define bezier_surface_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace std;

class BezierCurve
{
public:
    static float nCr(int n, int r)
    {
        if (r > n / 2) r = n - r;
        float res = 1;
        for (int i = 1; i <= r; i++) {
            res *= n - i + 1;
            res /= i;
        }
        return res;
    }

    static glm::vec3 evaluate(float t, const vector<glm::vec3>& points)
    {
        glm::vec3 p(0.0f);
        int n = points.size() - 1;
        for (int i = 0; i <= n; i++)
        {
            float f = nCr(n, i) * pow(t, i) * pow(1.0f - t, n - i);
            p += points[i] * f;
        }
        return p;
    }
};

class BezierSurface {
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    unsigned int textureMap;
    float shininess;

    BezierSurface(unsigned int tMap, const vector<glm::vec3>& controlPoints,
        glm::vec3 amb = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3 spec = glm::vec3(0.5f, 0.5f, 0.5f),
        float shiny = 32.0f, int sectors = 36, int stacks = 20)
    {
        this->textureMap = tMap;
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
        this->emissive = glm::vec3(0.0f, 0.0f, 0.0f);
        this->sectorCount = sectors;
        this->stackCount = stacks;
        this->controlPoints = controlPoints;

        setUpVertexDataAndConfigureVertexAttribute();
    }

    ~BezierSurface()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
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

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int vao, vbo, ebo;
    int sectorCount;
    int stackCount;
    int indexCount;
    vector<glm::vec3> controlPoints;

    void setUpVertexDataAndConfigureVertexAttribute()
    {
        vector<float> vertices;
        vector<unsigned int> indices;

        float sectorStep = 2 * 3.14159265359f / sectorCount;
        float stackStep = 1.0f / stackCount;

        // Note: computing tangents manually for normals.
        for (int i = 0; i <= stackCount; ++i)
        {
            float t = i * stackStep;
            glm::vec3 curvePoint = BezierCurve::evaluate(t, controlPoints);
            // approximate tangent for normal calculation
            float dt = 0.01f;
            glm::vec3 pNext = BezierCurve::evaluate(min(1.0f, t + dt), controlPoints);
            glm::vec3 pPrev = BezierCurve::evaluate(max(0.0f, t - dt), controlPoints);
            glm::vec3 tangent = glm::normalize(pNext - pPrev);

            // Curve is in XY plane, revolved around Y axis.
            // Tangent is in XY plane. Normal in XY plane is (-tangent.y, tangent.x, 0)
            glm::vec2 normal2D = glm::normalize(glm::vec2(-tangent.y, tangent.x));

            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;

                // Position revolved
                float x = curvePoint.x * cos(sectorAngle);
                float y = curvePoint.y;
                float z = curvePoint.x * sin(sectorAngle);

                // Normal revolved
                float nx = normal2D.x * cos(sectorAngle);
                float ny = normal2D.y;
                float nz = normal2D.x * sin(sectorAngle);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                vertices.push_back((float)j / sectorCount);
                vertices.push_back((float)i / stackCount);
            }
        }

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

        indexCount = indices.size();

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif /* bezier_surface_h */
