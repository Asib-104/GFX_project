#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "spotLight.h"
#include "cube.h"
#include "hexagon.h"
#include "pyramid.h"
#include "cylinder.h"
#include "sphere.h"
#include "cone.h"
#include "stb_image.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

// ─── Function Declarations ──────────────────────────────────────────────────
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);

// Auditorium draw helpers
void drawSeat(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel, float x, float y, float z);
void drawStage(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawTieredFloor(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawWalls(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawCeiling(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawWallPanels(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawRopeBarrier(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel, float x, float y, float z);
void drawRopeBarrierLine(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel);
void drawCeilingLights(Sphere& lightSphere, Shader& shader, Shader& lightShader, glm::mat4 baseModel);
void drawPodium(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel);
void drawTexturedObjects(Sphere& sphere1, Sphere& sphere2, Cone& cone1, Cone& cone2, Shader& shader, glm::mat4 baseModel);
void drawCurtains(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawProjectionScreen(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawWallSconces(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawFlags(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel);
void drawExteriorBuilding(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawExteriorGround(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawZigzagFins(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawExteriorWindows(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawBrickAccentWalls(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawEntrance(Cube& cube, Shader& shader, glm::mat4 baseModel, float doorAngle);
void drawRoofStructure(Cube& cube, Shader& shader, glm::mat4 baseModel);
void setCameraPreset(int preset);
void drawBezierWalkway(Cube& cube, Shader& shader, glm::mat4 baseModel);
void drawFractalTree(Cube& cube, Cube& cubeLeaf, Shader& shader, glm::mat4 parentTransform, float length, float thickness, int depth, int maxDepth);
void drawFractalTreeFull(Cube& cube, Cube& cubeLeaf, Shader& shader, glm::mat4 baseModel, float x, float z);
void drawDog(Cube& cube, Shader& shader, glm::mat4 baseModel, float x, float y, float z, float legAngle, int facing);
void printControls();
void setViewport(int mode, int width, int height);


// ─── Settings ───────────────────────────────────────────────────────────────
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 800;

// Modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// Camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 3.0, eyeZ = -3.0;
float lookAtX = 0.0, lookAtY = 2.0, lookAtZ = 10.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

int viewportMode = 0;
bool showTexturedObjects = true;
int textureMode = 0;

// ─── Lights ─────────────────────────────────────────────────────────────────
DirectionalLight directionalLight(
    -0.2f, -1.0f, -0.3f,
    0.15f, 0.14f, 0.12f,
    0.4f, 0.38f, 0.35f,
    0.5f, 0.5f, 0.5f
);

glm::vec3 pointLightPositions[] = {
    glm::vec3(-3.0f, 6.8f, 2.0f),
    glm::vec3(3.0f, 6.8f, 2.0f),
    glm::vec3(-3.0f, 7.5f, 10.0f),
    glm::vec3(3.0f, 7.5f, 10.0f)
};

PointLight pointlight1(pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z, 0.08f, 0.07f, 0.06f, 0.6f, 0.55f, 0.5f, 0.8f, 0.8f, 0.8f, 1.0f, 0.045f, 0.0075f, 1);
PointLight pointlight2(pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z, 0.08f, 0.07f, 0.06f, 0.6f, 0.55f, 0.5f, 0.8f, 0.8f, 0.8f, 1.0f, 0.045f, 0.0075f, 2);
PointLight pointlight3(pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z, 0.08f, 0.07f, 0.06f, 0.5f, 0.45f, 0.4f, 0.7f, 0.7f, 0.7f, 1.0f, 0.045f, 0.0075f, 3);
PointLight pointlight4(pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z, 0.08f, 0.07f, 0.06f, 0.5f, 0.45f, 0.4f, 0.7f, 0.7f, 0.7f, 1.0f, 0.045f, 0.0075f, 4);

SpotLight spotlight1(-2.0f, 6.5f, -1.0f, 0.2f, -1.0f, 0.3f, 0.05f, 0.05f, 0.04f, 1.0f, 0.95f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f, 0.09f, 0.032f, 15.0f, 20.0f, 1);
SpotLight spotlight2(2.0f, 6.5f, -1.0f, -0.2f, -1.0f, 0.3f, 0.05f, 0.05f, 0.04f, 1.0f, 0.95f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f, 0.09f, 0.032f, 15.0f, 20.0f, 2);

bool dirLightOn = true;
bool pointLightOn = true;
bool spotLightOn = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Door animation
bool doorOpen = false;
bool doorAnimating = false;
float doorAngle = 0.0f;          // 0 = closed, 90 = fully open
float doorSpeed = 80.0f;         // degrees per second

// Dog walking animation
bool dogsWalking = false;
float dogPosZ1 = 30.0f;         // Dog 1 position along Z (left side)
float dogPosZ2 = 33.0f;         // Dog 2 position along Z (right side, offset)
float dogLegAngle = 0.0f;       // Oscillating leg swing angle
float dogWalkSpeed = 2.5f;      // units per second
float dogPathMinZ = 18.0f;      // Turn-around point near building
float dogPathMaxZ = 36.0f;      // Turn-around point far end
int dogDir1 = -1;               // Dog 1 walking direction (-1 = toward building)
int dogDir2 = 1;                 // Dog 2 walking direction (+1 = away from building)

// ═══════════════════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════════════════
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        "Auditorium - CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // ─── Shaders ────────────────────────────────────────────────────────
    Shader lightingShaderWithTexture(
        "C:/Users/USER/source/repos/Lab-04/vertexShaderForPhongShadingWithTexture.vs",
        "C:/Users/USER/source/repos/Lab-04/fragmentShaderForPhongShadingWithTexture.fs");
    Shader ourShader(
        "C:/Users/USER/source/repos/Lab-04/vertexShader.vs",
        "C:/Users/USER/source/repos/Lab-04/fragmentShader.fs");

    // ─── Load ALL available textures ────────────────────────────────────
    string basePath = "C:/Users/USER/source/repos/Lab-04/";

    unsigned int texColor = loadTexture((basePath + "color.jpg").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texWood = loadTexture((basePath + "wood.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texCarpet = loadTexture((basePath + "carpet.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texCurtain = loadTexture((basePath + "curtain.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texMetal = loadTexture((basePath + "metal.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texSeatRed = loadTexture((basePath + "seat_red.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texWall = loadTexture((basePath + "wall.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texOffWhite = loadTexture((basePath + "offwhite.png").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texGrass = loadTexture((basePath + "grass.jpeg").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int texRoad = loadTexture((basePath + "road.jpeg").c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // ─── Geometry Objects (using different textures) ────────────────────
    Cube cubeWood = Cube(texWood);       // Stage, ceiling beams, wall panels
    Cube cubeCarpet = Cube(texCarpet);     // Floor / tiered seating carpet
    Cube cubeCurtain = Cube(texCurtain);    // Stage curtains
    Cube cubeMetal = Cube(texMetal);      // Metal elements, stanchions
    Cube cubeSeat = Cube(texSeatRed);    // Seat cushions (teal tinted via material)
    Cube cubeWall = Cube(texWall);       // Walls
    Cube cubeDefault = Cube(texColor);      // Default / misc
    Cube cubeOffWhite = Cube(texOffWhite);  // Off-white exterior concrete
    Cube cubeGrass = Cube(texGrass);      // Grass texture for exterior ground
    Cube cubeRoad = Cube(texRoad);        // Road/walkway texture

    Cylinder cylinder = Cylinder(texMetal);

    // Textured curvy surface objects (spheres & cones for the assignment)
    Sphere sphere1 = Sphere(texColor,
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f, 36, 18);
    Sphere sphere2 = Sphere(texWood,
        glm::vec3(0.2f, 0.3f, 0.6f), glm::vec3(0.3f, 0.4f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 64.0f, 36, 18);
    Cone cone1 = Cone(texColor,
        glm::vec3(0.8f, 0.2f, 0.1f), glm::vec3(0.8f, 0.2f, 0.1f), glm::vec3(0.8f, 0.2f, 0.1f), 32.0f, 36, 18);
    Cone cone2 = Cone(texWood,
        glm::vec3(0.2f, 0.7f, 0.3f), glm::vec3(0.2f, 0.7f, 0.3f), glm::vec3(0.2f, 0.7f, 0.3f), 64.0f, 36, 18);

    // Light indicator sphere
    Sphere lightSphere = Sphere(texColor,
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 32.0f, 18, 9);

    printControls();

    // ═════════════════════════════════════════════════════════════════════
    //  RENDER LOOP
    // ═════════════════════════════════════════════════════════════════════
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // ─── Door Animation Update ──────────────────────────────────────
        if (doorAnimating) {
            if (doorOpen) {
                doorAngle += doorSpeed * deltaTime;
                if (doorAngle >= 90.0f) { doorAngle = 90.0f; doorAnimating = false; }
            }
            else {
                doorAngle -= doorSpeed * deltaTime;
                if (doorAngle <= 0.0f) { doorAngle = 0.0f; doorAnimating = false; }
            }
        }

        glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setViewport(viewportMode, SCR_WIDTH, SCR_HEIGHT);

        // ─── Lighting Shader Setup ──────────────────────────────────────
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", basic_camera.eye);

        glm::mat4 projection = glm::perspective(glm::radians(basic_camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShaderWithTexture.setMat4("projection", projection);

        glm::mat4 view = basic_camera.createViewMatrix();
        lightingShaderWithTexture.setMat4("view", view);

        directionalLight.setUpDirectionalLight(lightingShaderWithTexture);
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        pointlight4.setUpPointLight(lightingShaderWithTexture);
        spotlight1.setUpSpotLight(lightingShaderWithTexture);
        spotlight2.setUpSpotLight(lightingShaderWithTexture);

        // ─── Model Transform ────────────────────────────────────────────
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glm::mat4 translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        glm::mat4 rotateXMatrix = glm::rotate(translateMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateYMatrix = glm::rotate(rotateXMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotateZMatrix = glm::rotate(rotateYMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::scale(rotateZMatrix, glm::vec3(scale_X, scale_Y, scale_Z));

        // ═════════════════════════════════════════════════════════════════
        //  DRAW AUDITORIUM (using different textured cubes)
        // ═════════════════════════════════════════════════════════════════

        drawStage(cubeWood, lightingShaderWithTexture, model);
        drawPodium(cubeDefault, cylinder, lightingShaderWithTexture, model);
        drawTieredFloor(cubeCarpet, lightingShaderWithTexture, model);
        drawWalls(cubeWall, lightingShaderWithTexture, model);
        drawWallPanels(cubeWood, lightingShaderWithTexture, model);
        drawCeiling(cubeDefault, lightingShaderWithTexture, model);
        drawCurtains(cubeCurtain, lightingShaderWithTexture, model);
        drawProjectionScreen(cubeDefault, lightingShaderWithTexture, model);
        drawWallSconces(cubeMetal, lightingShaderWithTexture, model);
        drawFlags(cubeDefault, cylinder, lightingShaderWithTexture, model);

        // Seats (10 rows, left+right of center aisle)
        // 7 seats per side (removed innermost column for wider aisle)
        for (int row = 0; row < 10; row++) {
            float rowZ = 1.0f + row * 1.4f;
            float rowY = 0.0f + row * 0.25f;
            for (int col = 0; col < 7; col++) {
                float seatX = -7.0f + col * 0.85f;
                drawSeat(cubeSeat, cylinder, lightingShaderWithTexture, model, seatX, rowY, rowZ);
            }
            for (int col = 0; col < 7; col++) {
                float seatX = 1.1f + col * 0.85f;
                drawSeat(cubeSeat, cylinder, lightingShaderWithTexture, model, seatX, rowY, rowZ);
            }
        }

        drawRopeBarrierLine(cubeMetal, cylinder, lightingShaderWithTexture, model);
        drawCeilingLights(lightSphere, lightingShaderWithTexture, ourShader, model);

        // ═════════════════════════════════════════════════════════════════
        //  DRAW EXTERIOR BUILDING
        //  cubeOffWhite = offwhite.png for all concrete surfaces
        //  cubeWall = wall.png for brick accent walls only
        //  cubeWood = wood.png for doors
        // ═════════════════════════════════════════════════════════════════
        drawExteriorBuilding(cubeOffWhite, lightingShaderWithTexture, model);
        drawExteriorGround(cubeGrass, lightingShaderWithTexture, model);
        drawZigzagFins(cubeOffWhite, lightingShaderWithTexture, model);
        drawExteriorWindows(cubeOffWhite, lightingShaderWithTexture, model);
        drawBrickAccentWalls(cubeWall, lightingShaderWithTexture, model);
        drawEntrance(cubeWood, lightingShaderWithTexture, model, doorAngle);
        drawRoofStructure(cubeOffWhite, lightingShaderWithTexture, model);

        // ═════════════════════════════════════════════════════════════════
        //  BEZIER WALKWAY + FRACTAL TREES (in front of auditorium)
        // ═════════════════════════════════════════════════════════════════
        drawBezierWalkway(cubeRoad, lightingShaderWithTexture, model);
        drawFractalTreeFull(cubeWood, cubeGrass, lightingShaderWithTexture, model, -9.0f, 24.0f);   // Left tree
        drawFractalTreeFull(cubeWood, cubeGrass, lightingShaderWithTexture, model, 9.0f, 24.0f);    // Right tree

        // ═════════════════════════════════════════════════════════════════
        //  ANIMATED DOGS (walking beside the road)
        // ═════════════════════════════════════════════════════════════════
        if (dogsWalking) {
            // Update leg swing animation
            dogLegAngle += 300.0f * deltaTime;
            if (dogLegAngle > 360.0f) dogLegAngle -= 360.0f;

            // Move dog 1 (left side of walkway)
            dogPosZ1 += dogDir1 * dogWalkSpeed * deltaTime;
            if (dogPosZ1 <= dogPathMinZ) { dogDir1 = 1; }
            if (dogPosZ1 >= dogPathMaxZ) { dogDir1 = -1; }

            // Move dog 2 (right side of walkway)
            dogPosZ2 += dogDir2 * dogWalkSpeed * deltaTime;
            if (dogPosZ2 <= dogPathMinZ) { dogDir2 = 1; }
            if (dogPosZ2 >= dogPathMaxZ) { dogDir2 = -1; }
        }

        // Draw dog 1 (left side, brown)
        drawDog(cubeWood, lightingShaderWithTexture, model, -4.5f, 0.0f, dogPosZ1, dogsWalking ? dogLegAngle : 0.0f, dogDir1);
        // Draw dog 2 (right side, brown)
        drawDog(cubeWood, lightingShaderWithTexture, model, 4.5f, 0.0f, dogPosZ2, dogsWalking ? dogLegAngle + 180.0f : 0.0f, dogDir2);

        // Textured objects moved AWAY from stage (only in audience area if enabled)
        if (showTexturedObjects) {
            drawTexturedObjects(sphere1, sphere2, cone1, cone2, lightingShaderWithTexture, model);
        }

        // ─── Light Bulb Indicators ──────────────────────────────────────
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        for (unsigned int i = 0; i < 4; i++) {
            Cube lightCube = Cube(glm::vec3(1.0f, 0.95f, 0.8f));
            glm::mat4 lm = glm::mat4(1.0f);
            lm = glm::translate(lm, pointLightPositions[i]);
            lm = glm::scale(lm, glm::vec3(0.15f));
            lightCube.drawColor(ourShader, lm);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
//  INPUT PROCESSING
// ═══════════════════════════════════════════════════════════════════════════
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) basic_camera.move(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) basic_camera.move(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) basic_camera.move(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) basic_camera.move(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (rotateAxis_X) rotateAngle_X -= 0.1;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1;
        else rotateAngle_Z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) { rotateAngle_X += 0.1; rotateAxis_X = 1.0; rotateAxis_Y = 0.0; rotateAxis_Z = 0.0; }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) { rotateAngle_Y += 0.1; rotateAxis_X = 0.0; rotateAxis_Y = 1.0; rotateAxis_Z = 0.0; }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) { rotateAngle_Z += 0.1; rotateAxis_X = 0.0; rotateAxis_Y = 0.0; rotateAxis_Z = 1.0; }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) { eyeX += 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) { eyeX -= 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) { eyeZ += 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { eyeZ -= 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { eyeY += 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { eyeY -= 2.5 * deltaTime; basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ); }

    int rot_unit = 100;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    basic_camera.Pitch += rot_unit * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  basic_camera.Pitch -= rot_unit * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  basic_camera.Yaw -= rot_unit * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) basic_camera.Yaw += rot_unit * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)  basic_camera.Roll -= rot_unit * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) basic_camera.Roll += rot_unit * deltaTime;
}

// ═══════════════════════════════════════════════════════════════════════════
//  CAMERA PRESETS
// ═══════════════════════════════════════════════════════════════════════════
void setCameraPreset(int preset)
{
    // Reset pitch/yaw/roll for clean preset views
    basic_camera.Pitch = 0.0f;
    basic_camera.Yaw = 0.0f;
    basic_camera.Roll = 0.0f;

    switch (preset) {
    case 1: // Stage view → looking at audience (speaker's POV)
        eyeX = 0.0f; eyeY = 2.5f; eyeZ = -2.0f;
        lookAtX = 0.0f; lookAtY = 2.0f; lookAtZ = 10.0f;
        cout << "[F1] Stage View (Speaker POV)\n"; break;
    case 2: // Audience center → looking at stage (front row center)
        eyeX = 0.0f; eyeY = 1.5f; eyeZ = 2.0f;
        lookAtX = 0.0f; lookAtY = 2.5f; lookAtZ = -3.0f;
        cout << "[F2] Front Row Center\n"; break;
    case 3: // Audience back center → looking at stage
        eyeX = 0.0f; eyeY = 4.0f; eyeZ = 14.0f;
        lookAtX = 0.0f; lookAtY = 2.0f; lookAtZ = -3.0f;
        cout << "[F3] Back Row Center\n"; break;
    case 4: // Left side view (from left wall looking across)
        eyeX = -7.5f; eyeY = 3.0f; eyeZ = 5.0f;
        lookAtX = 3.0f; lookAtY = 2.0f; lookAtZ = -1.0f;
        cout << "[F4] Left Side View\n"; break;
    case 5: // Right side view (from right wall looking across)
        eyeX = 7.5f; eyeY = 3.0f; eyeZ = 5.0f;
        lookAtX = -3.0f; lookAtY = 2.0f; lookAtZ = -1.0f;
        cout << "[F5] Right Side View\n"; break;
    case 6: // Top-down bird's eye (directly above center)
        eyeX = 0.0f; eyeY = 12.0f; eyeZ = 5.0f;
        lookAtX = 0.0f; lookAtY = 0.0f; lookAtZ = 5.0f;
        cout << "[F6] Bird's Eye (Top Down)\n"; break;
    case 7: // Balcony / high back view
        eyeX = 0.0f; eyeY = 7.0f; eyeZ = 15.0f;
        lookAtX = 0.0f; lookAtY = 1.0f; lookAtZ = -3.0f;
        cout << "[F7] High Back / Balcony View\n"; break;
    case 8: // Close-up of stage from audience left
        eyeX = -4.0f; eyeY = 1.8f; eyeZ = 1.5f;
        lookAtX = 0.0f; lookAtY = 2.5f; lookAtZ = -3.0f;
        cout << "[F8] Audience Left Close-up\n"; break;
    case 9: // Close-up of stage from audience right
        eyeX = 4.0f; eyeY = 1.8f; eyeZ = 1.5f;
        lookAtX = 0.0f; lookAtY = 2.5f; lookAtZ = -3.0f;
        cout << "[F9] Audience Right Close-up\n"; break;
    case 10: // Podium close-up (from stage)
        eyeX = -2.0f; eyeY = 2.0f; eyeZ = -1.5f;
        lookAtX = -3.0f; lookAtY = 1.8f; lookAtZ = -2.0f;
        cout << "[F10] Podium Close-up\n"; break;
    case 11: // Isometric 3/4 view (front-left elevated)
        eyeX = -10.0f; eyeY = 8.0f; eyeZ = -5.0f;
        lookAtX = 0.0f; lookAtY = 1.5f; lookAtZ = 5.0f;
        cout << "[F11] Isometric Front-Left\n"; break;
    case 12: // Isometric 3/4 view (back-right elevated)
        eyeX = 10.0f; eyeY = 8.0f; eyeZ = 16.0f;
        lookAtX = 0.0f; lookAtY = 1.5f; lookAtZ = 3.0f;
        cout << "[F12] Isometric Back-Right\n"; break;
    case 13: // Exterior Front view (standing outside looking at building)
        eyeX = 0.0f; eyeY = 5.0f; eyeZ = 30.0f;
        lookAtX = 0.0f; lookAtY = 4.0f; lookAtZ = 15.0f;
        cout << "[SHIFT+F1] Exterior Front View\n"; break;
    case 14: // Exterior Left side
        eyeX = -20.0f; eyeY = 5.0f; eyeZ = 5.0f;
        lookAtX = 0.0f; lookAtY = 4.0f; lookAtZ = 5.0f;
        cout << "[SHIFT+F2] Exterior Left Side\n"; break;
    case 15: // Exterior Right side
        eyeX = 20.0f; eyeY = 5.0f; eyeZ = 5.0f;
        lookAtX = 0.0f; lookAtY = 4.0f; lookAtZ = 5.0f;
        cout << "[SHIFT+F3] Exterior Right Side\n"; break;
    case 16: // Exterior Back (behind stage wall)
        eyeX = 0.0f; eyeY = 5.0f; eyeZ = -18.0f;
        lookAtX = 0.0f; lookAtY = 4.0f; lookAtZ = -5.0f;
        cout << "[SHIFT+F4] Exterior Back View\n"; break;
    case 17: // Exterior aerial / drone view
        eyeX = -15.0f; eyeY = 18.0f; eyeZ = 25.0f;
        lookAtX = 0.0f; lookAtY = 3.0f; lookAtZ = 5.0f;
        cout << "[SHIFT+F5] Exterior Aerial / Drone\n"; break;
    case 18: // Exterior entrance close-up
        eyeX = 0.0f; eyeY = 2.5f; eyeZ = 22.0f;
        lookAtX = 0.0f; lookAtY = 3.0f; lookAtZ = 15.0f;
        cout << "[SHIFT+F6] Entrance Close-up\n"; break;
    default: break;
    }

    basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
}

// ═══════════════════════════════════════════════════════════════════════════
//  KEY CALLBACK
// ═══════════════════════════════════════════════════════════════════════════
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // ─── SHIFT + F1-F6: Exterior Camera Presets (check FIRST) ─────────
    if (mods & GLFW_MOD_SHIFT) {
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) { setCameraPreset(13); return; }
        if (key == GLFW_KEY_F2 && action == GLFW_PRESS) { setCameraPreset(14); return; }
        if (key == GLFW_KEY_F3 && action == GLFW_PRESS) { setCameraPreset(15); return; }
        if (key == GLFW_KEY_F4 && action == GLFW_PRESS) { setCameraPreset(16); return; }
        if (key == GLFW_KEY_F5 && action == GLFW_PRESS) { setCameraPreset(17); return; }
        if (key == GLFW_KEY_F6 && action == GLFW_PRESS) { setCameraPreset(18); return; }
    }

    // ─── F1-F12: Interior Camera Presets ────────────────────────────────
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) { setCameraPreset(1); return; }
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) { setCameraPreset(2); return; }
    if (key == GLFW_KEY_F3 && action == GLFW_PRESS) { setCameraPreset(3); return; }
    if (key == GLFW_KEY_F4 && action == GLFW_PRESS) { setCameraPreset(4); return; }
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS) { setCameraPreset(5); return; }
    if (key == GLFW_KEY_F6 && action == GLFW_PRESS) { setCameraPreset(6); return; }
    if (key == GLFW_KEY_F7 && action == GLFW_PRESS) { setCameraPreset(7); return; }
    if (key == GLFW_KEY_F8 && action == GLFW_PRESS) { setCameraPreset(8); return; }
    if (key == GLFW_KEY_F9 && action == GLFW_PRESS) { setCameraPreset(9); return; }
    if (key == GLFW_KEY_F10 && action == GLFW_PRESS) { setCameraPreset(10); return; }
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) { setCameraPreset(11); return; }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) { setCameraPreset(12); return; }

    // ─── Lighting Toggles ───────────────────────────────────────────────
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (dirLightOn) { directionalLight.turnOff(); dirLightOn = false; cout << "Directional Light OFF\n"; }
        else { directionalLight.turnOn(); dirLightOn = true; cout << "Directional Light ON\n"; }
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        if (pointLightOn) {
            pointlight1.turnOff(); pointlight2.turnOff(); pointlight3.turnOff(); pointlight4.turnOff();
            pointLightOn = false; cout << "Point Lights OFF\n";
        }
        else {
            pointlight1.turnOn(); pointlight2.turnOn(); pointlight3.turnOn(); pointlight4.turnOn();
            pointLightOn = true; cout << "Point Lights ON\n";
        }
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        if (spotLightOn) { spotlight1.turnOff(); spotlight2.turnOff(); spotLightOn = false; cout << "Spot Lights OFF\n"; }
        else { spotlight1.turnOn(); spotlight2.turnOn(); spotLightOn = true; cout << "Spot Lights ON\n"; }
    }
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        if (ambientToggle) {
            directionalLight.turnAmbientOff(); pointlight1.turnAmbientOff(); pointlight2.turnAmbientOff();
            pointlight3.turnAmbientOff(); pointlight4.turnAmbientOff(); spotlight1.turnAmbientOff(); spotlight2.turnAmbientOff();
            ambientToggle = false; cout << "Ambient Light OFF\n";
        }
        else {
            directionalLight.turnAmbientOn(); pointlight1.turnAmbientOn(); pointlight2.turnAmbientOn();
            pointlight3.turnAmbientOn(); pointlight4.turnAmbientOn(); spotlight1.turnAmbientOn(); spotlight2.turnAmbientOn();
            ambientToggle = true; cout << "Ambient Light ON\n";
        }
    }
    else if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
        if (diffuseToggle) {
            directionalLight.turnDiffuseOff(); pointlight1.turnDiffuseOff(); pointlight2.turnDiffuseOff();
            pointlight3.turnDiffuseOff(); pointlight4.turnDiffuseOff(); spotlight1.turnDiffuseOff(); spotlight2.turnDiffuseOff();
            diffuseToggle = false; cout << "Diffuse Light OFF\n";
        }
        else {
            directionalLight.turnDiffuseOn(); pointlight1.turnDiffuseOn(); pointlight2.turnDiffuseOn();
            pointlight3.turnDiffuseOn(); pointlight4.turnDiffuseOn(); spotlight1.turnDiffuseOn(); spotlight2.turnDiffuseOn();
            diffuseToggle = true; cout << "Diffuse Light ON\n";
        }
    }
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
        if (specularToggle) {
            directionalLight.turnSpecularOff(); pointlight1.turnSpecularOff(); pointlight2.turnSpecularOff();
            pointlight3.turnSpecularOff(); pointlight4.turnSpecularOff(); spotlight1.turnSpecularOff(); spotlight2.turnSpecularOff();
            specularToggle = false; cout << "Specular Light OFF\n";
        }
        else {
            directionalLight.turnSpecularOn(); pointlight1.turnSpecularOn(); pointlight2.turnSpecularOn();
            pointlight3.turnSpecularOn(); pointlight4.turnSpecularOn(); spotlight1.turnSpecularOn(); spotlight2.turnSpecularOn();
            specularToggle = true; cout << "Specular Light ON\n";
        }
    }
    // ─── Viewport / Texture / Object Toggles ────────────────────────────
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS) { viewportMode = (viewportMode + 1) % 9; cout << "Viewport Mode: " << viewportMode << endl; }
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS) { showTexturedObjects = !showTexturedObjects; cout << "Textured Objects: " << (showTexturedObjects ? "ON" : "OFF") << endl; }
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
        textureMode = (textureMode + 1) % 5;
        string modes[] = { "All Objects", "Simple Texture Only", "Blended Texture Only", "Vertex Color Only", "Fragment Color Only" };
        cout << "Texture Mode: " << modes[textureMode] << endl;
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS) { textureMode = 0; showTexturedObjects = true; cout << "Reset: Showing all textured objects\n"; }
    // ─── Camera Rotation (discrete steps) ───────────────────────────────
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        doorOpen = !doorOpen;
        doorAnimating = true;
        cout << "Door " << (doorOpen ? "OPENING" : "CLOSING") << "\n";
    }
    else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        dogsWalking = !dogsWalking;
        cout << "Dogs " << (dogsWalking ? "WALKING" : "STOPPED") << "\n";
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS) { basic_camera.Pitch += 5.0f; }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) { basic_camera.Pitch -= 5.0f; }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) { basic_camera.Yaw -= 5.0f; }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) { basic_camera.Yaw += 5.0f; }
    else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) { basic_camera.Roll -= 5.0f; }
    else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) { basic_camera.Roll += 5.0f; }
}

// ═══════════════════════════════════════════════════════════════════════════
//  CALLBACKS
// ═══════════════════════════════════════════════════════════════════════════
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { SCR_WIDTH = width; SCR_HEIGHT = height; glViewport(0, 0, width, height); }

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    // Calculate mouse offset from last position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Reversed because y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Sensitivity multiplier for smoother control
    float sensitivity = 0.15f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update camera rotation (Yaw and Pitch) - 360 degree freedom
    basic_camera.Yaw += xoffset;
    basic_camera.Pitch += yoffset;

    // Normalize yaw to 0-360 range (allows full 360 degree rotation)
    while (basic_camera.Yaw > 360.0f)
        basic_camera.Yaw -= 360.0f;
    while (basic_camera.Yaw < 0.0f)
        basic_camera.Yaw += 360.0f;

    // Clamp pitch to -180 to 180 (allows full 360 degree vertical rotation, no gimbal lock)
    while (basic_camera.Pitch > 180.0f)
        basic_camera.Pitch -= 360.0f;
    while (basic_camera.Pitch < -180.0f)
        basic_camera.Pitch += 360.0f;

    // Calculate new camera direction based on Yaw and Pitch (spherical coordinates)
    glm::vec3 direction;
    direction.x = cos(glm::radians(basic_camera.Yaw)) * cos(glm::radians(basic_camera.Pitch));
    direction.y = sin(glm::radians(basic_camera.Pitch));
    direction.z = sin(glm::radians(basic_camera.Yaw)) * cos(glm::radians(basic_camera.Pitch));
    direction = glm::normalize(direction);

    // Update lookAt position based on camera direction and distance
    float viewDistance = 10.0f;  // Distance to look ahead
    basic_camera.lookAt = basic_camera.eye + direction * viewDistance;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { basic_camera.ProcessMouseScroll(static_cast<float>(yoffset)); }

// ═══════════════════════════════════════════════════════════════════════════
//  TEXTURE LOADER
// ═══════════════════════════════════════════════════════════════════════════
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);
        stbi_image_free(data);
    }
    else {
        cout << "Texture failed to load at path: " << path << endl;
        stbi_image_free(data);
    }
    return textureID;
}

// ═══════════════════════════════════════════════════════════════════════════
//  DRAW FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

// ─── SEAT (teal cushion + dark brown frame, using seat texture) ─────────
void drawSeat(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel, float x, float y, float z)
{
    // Translate to seat position, then rotate 180° around Y so seat faces stage (negative Z)
    glm::mat4 seatOrigin = glm::translate(baseModel, glm::vec3(x, y, z));
    seatOrigin = glm::rotate(seatOrigin, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Seat cushion (teal/turquoise)
    cube.ambient = glm::vec3(0.1f, 0.35f, 0.32f);
    cube.diffuse = glm::vec3(0.2f, 0.65f, 0.6f);
    cube.specular = glm::vec3(0.3f, 0.4f, 0.4f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(seatOrigin, glm::vec3(0.0f, 0.45f, 0.0f));
    m = glm::scale(m, glm::vec3(0.55f, 0.1f, 0.5f));
    cube.draw(shader, m);

    // Seat back (dark grey/brown)
    cube.ambient = glm::vec3(0.08f, 0.07f, 0.06f);
    cube.diffuse = glm::vec3(0.18f, 0.16f, 0.14f);
    cube.specular = glm::vec3(0.15f, 0.15f, 0.15f);
    m = glm::translate(seatOrigin, glm::vec3(0.0f, 0.75f, -0.2f));
    m = glm::rotate(m, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::scale(m, glm::vec3(0.55f, 0.55f, 0.08f));
    cube.draw(shader, m);

    // Seat back top cushion (teal accent)
    cube.ambient = glm::vec3(0.1f, 0.32f, 0.3f);
    cube.diffuse = glm::vec3(0.18f, 0.58f, 0.55f);
    m = glm::translate(seatOrigin, glm::vec3(0.0f, 1.05f, -0.22f));
    m = glm::scale(m, glm::vec3(0.5f, 0.08f, 0.1f));
    cube.draw(shader, m);

    // Legs (dark metal)
    cube.ambient = glm::vec3(0.08f, 0.08f, 0.08f);
    cube.diffuse = glm::vec3(0.15f, 0.15f, 0.15f);
    cube.specular = glm::vec3(0.6f, 0.6f, 0.6f);
    m = glm::translate(seatOrigin, glm::vec3(-0.22f, 0.2f, -0.15f)); m = glm::scale(m, glm::vec3(0.04f, 0.4f, 0.04f)); cube.draw(shader, m);
    m = glm::translate(seatOrigin, glm::vec3(0.22f, 0.2f, -0.15f));  m = glm::scale(m, glm::vec3(0.04f, 0.4f, 0.04f)); cube.draw(shader, m);
    m = glm::translate(seatOrigin, glm::vec3(-0.22f, 0.2f, 0.15f));  m = glm::scale(m, glm::vec3(0.04f, 0.4f, 0.04f)); cube.draw(shader, m);
    m = glm::translate(seatOrigin, glm::vec3(0.22f, 0.2f, 0.15f));   m = glm::scale(m, glm::vec3(0.04f, 0.4f, 0.04f)); cube.draw(shader, m);

    // Armrests (dark grey)
    cube.ambient = glm::vec3(0.06f, 0.06f, 0.06f);
    cube.diffuse = glm::vec3(0.14f, 0.14f, 0.14f);
    cube.specular = glm::vec3(0.2f, 0.2f, 0.2f);
    m = glm::translate(seatOrigin, glm::vec3(-0.28f, 0.55f, -0.05f)); m = glm::scale(m, glm::vec3(0.04f, 0.04f, 0.35f)); cube.draw(shader, m);
    m = glm::translate(seatOrigin, glm::vec3(0.28f, 0.55f, -0.05f));  m = glm::scale(m, glm::vec3(0.04f, 0.04f, 0.35f)); cube.draw(shader, m);
}

// ─── STAGE (light oak wood platform) ────────────────────────────────────
void drawStage(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = glm::vec3(0.5f, 0.38f, 0.2f);
    cube.diffuse = glm::vec3(0.82f, 0.65f, 0.4f);
    cube.specular = glm::vec3(0.45f, 0.38f, 0.25f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 0.4f, -2.5f));
    m = glm::scale(m, glm::vec3(16.0f, 0.8f, 5.0f));
    cube.draw(shader, m);

    // Wood plank lines
    cube.ambient = glm::vec3(0.42f, 0.32f, 0.17f);
    cube.diffuse = glm::vec3(0.7f, 0.55f, 0.33f);
    for (int i = -3; i <= 3; i++) {
        m = glm::translate(baseModel, glm::vec3(i * 2.0f, 0.82f, -2.5f));
        m = glm::scale(m, glm::vec3(0.03f, 0.02f, 5.0f));
        cube.draw(shader, m);
    }

    // Stage front face
    cube.ambient = glm::vec3(0.38f, 0.3f, 0.16f);
    cube.diffuse = glm::vec3(0.65f, 0.5f, 0.3f);
    m = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, 0.01f));
    m = glm::scale(m, glm::vec3(16.0f, 0.8f, 0.02f));
    cube.draw(shader, m);

    // Seating area floor (grey/beige carpet)
    cube.ambient = glm::vec3(0.18f, 0.17f, 0.15f);
    cube.diffuse = glm::vec3(0.35f, 0.33f, 0.3f);
    cube.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    m = glm::translate(baseModel, glm::vec3(0.0f, -0.01f, 7.0f));
    m = glm::scale(m, glm::vec3(16.0f, 0.02f, 15.0f));
    cube.draw(shader, m);
}

// ─── CURTAINS (dark charcoal fabric on stage sides) ─────────────────────
void drawCurtains(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = glm::vec3(0.04f, 0.04f, 0.05f);
    cube.diffuse = glm::vec3(0.1f, 0.1f, 0.12f);
    cube.specular = glm::vec3(0.06f, 0.06f, 0.06f);
    cube.emissive = glm::vec3(0.0f);

    // Left curtain
    glm::mat4 m = glm::translate(baseModel, glm::vec3(-6.5f, 3.5f, -3.0f));
    m = glm::scale(m, glm::vec3(2.0f, 5.5f, 4.0f));
    cube.draw(shader, m);

    // Right curtain
    m = glm::translate(baseModel, glm::vec3(6.5f, 3.5f, -3.0f));
    m = glm::scale(m, glm::vec3(2.0f, 5.5f, 4.0f));
    cube.draw(shader, m);

    // Top valance
    cube.ambient = glm::vec3(0.05f, 0.05f, 0.06f);
    cube.diffuse = glm::vec3(0.12f, 0.12f, 0.14f);
    m = glm::translate(baseModel, glm::vec3(0.0f, 6.0f, -3.0f));
    m = glm::scale(m, glm::vec3(11.0f, 0.8f, 0.25f));
    cube.draw(shader, m);

    // Curtain fold lines (subtle vertical dark strips for texture)
    cube.ambient = glm::vec3(0.06f, 0.06f, 0.07f);
    cube.diffuse = glm::vec3(0.14f, 0.14f, 0.16f);
    for (int i = 0; i < 4; i++) {
        m = glm::translate(baseModel, glm::vec3(-6.5f + i * 0.4f, 3.5f, -1.05f));
        m = glm::scale(m, glm::vec3(0.07f, 5.0f, 0.07f));
        cube.draw(shader, m);
    }
    for (int i = 0; i < 4; i++) {
        m = glm::translate(baseModel, glm::vec3(5.7f + i * 0.4f, 3.5f, -1.05f));
        m = glm::scale(m, glm::vec3(0.07f, 5.0f, 0.07f));
        cube.draw(shader, m);
    }
}

// ─── PROJECTION SCREEN (white screen + dark frame on back stage wall) ───
void drawProjectionScreen(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    // White screen surface
    cube.ambient = glm::vec3(0.55f, 0.55f, 0.55f);
    cube.diffuse = glm::vec3(0.88f, 0.88f, 0.88f);
    cube.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    cube.emissive = glm::vec3(0.04f, 0.04f, 0.04f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 3.8f, -5.05f));
    m = glm::scale(m, glm::vec3(7.5f, 4.2f, 0.06f));
    cube.draw(shader, m);

    // Dark border frame
    cube.ambient = glm::vec3(0.04f, 0.04f, 0.04f);
    cube.diffuse = glm::vec3(0.08f, 0.08f, 0.08f);
    cube.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    cube.emissive = glm::vec3(0.0f);

    // Top
    m = glm::translate(baseModel, glm::vec3(0.0f, 5.95f, -5.02f));
    m = glm::scale(m, glm::vec3(7.7f, 0.1f, 0.1f));
    cube.draw(shader, m);
    // Bottom
    m = glm::translate(baseModel, glm::vec3(0.0f, 1.65f, -5.02f));
    m = glm::scale(m, glm::vec3(7.7f, 0.1f, 0.1f));
    cube.draw(shader, m);
    // Left
    m = glm::translate(baseModel, glm::vec3(-3.85f, 3.8f, -5.02f));
    m = glm::scale(m, glm::vec3(0.1f, 4.4f, 0.1f));
    cube.draw(shader, m);
    // Right
    m = glm::translate(baseModel, glm::vec3(3.85f, 3.8f, -5.02f));
    m = glm::scale(m, glm::vec3(0.1f, 4.4f, 0.1f));
    cube.draw(shader, m);
}

// ─── WALL SCONCE LIGHTS (warm glow, between wall panels) ────────────────
void drawWallSconces(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    for (int side = 0; side < 2; side++)
    {
        float wallX = (side == 0) ? -7.72f : 7.72f;

        for (int i = 0; i < 4; i++)
        {
            float sconceZ = -1.0f + i * 4.0f + 2.0f;  // between panel pillars

            // Fixture body (dark metal)
            cube.ambient = glm::vec3(0.06f, 0.06f, 0.06f);
            cube.diffuse = glm::vec3(0.14f, 0.14f, 0.14f);
            cube.specular = glm::vec3(0.35f, 0.35f, 0.35f);
            cube.emissive = glm::vec3(0.0f);

            glm::mat4 m = glm::translate(baseModel, glm::vec3(wallX, 4.2f, sconceZ));
            m = glm::scale(m, glm::vec3(0.1f, 0.25f, 0.1f));
            cube.draw(shader, m);

            // Warm upper glow
            cube.ambient = glm::vec3(0.55f, 0.42f, 0.18f);
            cube.diffuse = glm::vec3(0.75f, 0.58f, 0.28f);
            cube.specular = glm::vec3(0.4f, 0.35f, 0.18f);
            cube.emissive = glm::vec3(0.35f, 0.25f, 0.1f);

            m = glm::translate(baseModel, glm::vec3(wallX, 4.42f, sconceZ));
            m = glm::scale(m, glm::vec3(0.07f, 0.12f, 0.07f));
            cube.draw(shader, m);

            // Warm lower glow
            m = glm::translate(baseModel, glm::vec3(wallX, 3.98f, sconceZ));
            m = glm::scale(m, glm::vec3(0.07f, 0.12f, 0.07f));
            cube.draw(shader, m);
        }
    }
    cube.emissive = glm::vec3(0.0f);
}

// ─── FLAGS (on stage sides like reference images 5-6) ───────────────────
void drawFlags(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel)
{
    // Left flag pole (chrome)
    cube.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    cube.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    cube.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(-5.0f, 2.3f, -4.0f));
    m = glm::scale(m, glm::vec3(0.04f, 2.8f, 0.04f));
    cube.draw(shader, m);

    // Left flag base
    cube.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    cube.diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
    m = glm::translate(baseModel, glm::vec3(-5.0f, 0.9f, -4.0f));
    m = glm::scale(m, glm::vec3(0.22f, 0.08f, 0.22f));
    cube.draw(shader, m);

    // Left flag cloth (white)
    cube.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    cube.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    cube.specular = glm::vec3(0.2f, 0.2f, 0.2f);
    m = glm::translate(baseModel, glm::vec3(-4.72f, 3.1f, -4.0f));
    m = glm::scale(m, glm::vec3(0.5f, 0.9f, 0.02f));
    cube.draw(shader, m);

    // Right flag pole
    cube.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    cube.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
    cube.specular = glm::vec3(0.8f, 0.8f, 0.8f);
    m = glm::translate(baseModel, glm::vec3(5.0f, 2.3f, -4.0f));
    m = glm::scale(m, glm::vec3(0.04f, 2.8f, 0.04f));
    cube.draw(shader, m);

    // Right flag base
    cube.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    cube.diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
    m = glm::translate(baseModel, glm::vec3(5.0f, 0.9f, -4.0f));
    m = glm::scale(m, glm::vec3(0.22f, 0.08f, 0.22f));
    cube.draw(shader, m);

    // Right flag cloth (red)
    cube.ambient = glm::vec3(0.4f, 0.05f, 0.05f);
    cube.diffuse = glm::vec3(0.7f, 0.1f, 0.1f);
    cube.specular = glm::vec3(0.2f, 0.1f, 0.1f);
    m = glm::translate(baseModel, glm::vec3(5.28f, 3.1f, -4.0f));
    m = glm::scale(m, glm::vec3(0.5f, 0.9f, 0.02f));
    cube.draw(shader, m);
}

// ─── PODIUM ─────────────────────────────────────────────────────────────
void drawPodium(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = glm::vec3(0.05f, 0.04f, 0.03f);
    cube.diffuse = glm::vec3(0.12f, 0.1f, 0.08f);
    cube.specular = glm::vec3(0.3f, 0.3f, 0.3f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(-3.0f, 1.3f, -2.0f));
    m = glm::scale(m, glm::vec3(0.6f, 1.0f, 0.4f));
    cube.draw(shader, m);

    cube.ambient = glm::vec3(0.08f, 0.07f, 0.05f);
    cube.diffuse = glm::vec3(0.18f, 0.15f, 0.12f);
    m = glm::translate(baseModel, glm::vec3(-3.0f, 1.85f, -2.0f));
    m = glm::scale(m, glm::vec3(0.7f, 0.1f, 0.5f));
    cube.draw(shader, m);

    // Microphone
    cube.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    cube.diffuse = glm::vec3(0.25f, 0.25f, 0.25f);
    cube.specular = glm::vec3(0.7f, 0.7f, 0.7f);
    m = glm::translate(baseModel, glm::vec3(-3.0f, 2.3f, -1.9f));
    m = glm::scale(m, glm::vec3(0.02f, 0.4f, 0.02f));
    cube.draw(shader, m);
}

// ─── TIERED FLOOR ───────────────────────────────────────────────────────
void drawTieredFloor(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);
    for (int row = 0; row < 10; row++) {
        float rowZ = 1.0f + row * 1.4f;
        float rowY = row * 0.25f;

        if (row > 0) {
            cube.ambient = glm::vec3(0.1f, 0.1f, 0.09f);
            cube.diffuse = glm::vec3(0.2f, 0.19f, 0.18f);
            cube.specular = glm::vec3(0.05f, 0.05f, 0.05f);
            glm::mat4 riser = glm::translate(baseModel, glm::vec3(0.0f, rowY - 0.125f, rowZ - 0.7f));
            riser = glm::scale(riser, glm::vec3(16.0f, 0.25f, 0.05f));
            cube.draw(shader, riser);
        }

        cube.ambient = glm::vec3(0.16f, 0.15f, 0.13f);
        cube.diffuse = glm::vec3(0.3f, 0.28f, 0.25f);
        cube.specular = glm::vec3(0.08f, 0.08f, 0.08f);
        glm::mat4 tread = glm::translate(baseModel, glm::vec3(0.0f, rowY - 0.01f, rowZ));
        tread = glm::scale(tread, glm::vec3(16.0f, 0.02f, 1.4f));
        cube.draw(shader, tread);
    }
}

// ─── WALLS ──────────────────────────────────────────────────────────────
void drawWalls(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = glm::vec3(0.3f, 0.28f, 0.22f);
    cube.diffuse = glm::vec3(0.6f, 0.55f, 0.45f);
    cube.specular = glm::vec3(0.2f, 0.2f, 0.18f);
    cube.emissive = glm::vec3(0.0f);

    // BACK WALL (audience side, Z=15.5) with 3 door openings
    // Doors at X = -3, 0, +3 (each 2.0 wide, 3.6 tall from Y=0 to Y=3.6)
    // 
    // Wall layout (looking from inside):
    //   X: -8......-4  -4..-2  -2..-1  -1..+1  +1..+2  +2..+4  +4......+8
    //       WALL      DOOR1    WALL    DOOR2    WALL    DOOR3     WALL
    //
    // Upper portion (above doors, full width): Y=3.6 to Y=7.0
    glm::mat4 m;

    m = glm::translate(baseModel, glm::vec3(0.0f, 5.3f, 15.5f));
    m = glm::scale(m, glm::vec3(16.0f, 3.4f, 0.3f));
    cube.draw(shader, m);

    // Far left wall segment: X = -8 to -4 (center -6, width 4)
    m = glm::translate(baseModel, glm::vec3(-6.0f, 1.8f, 15.5f));
    m = glm::scale(m, glm::vec3(4.0f, 3.6f, 0.3f));
    cube.draw(shader, m);

    // Pillar between door1 and door2: X = -2 to -1 (center -1.5, width 1)
    m = glm::translate(baseModel, glm::vec3(-1.5f, 1.8f, 15.5f));
    m = glm::scale(m, glm::vec3(1.0f, 3.6f, 0.3f));
    cube.draw(shader, m);

    // Pillar between door2 and door3: X = +1 to +2 (center +1.5, width 1)
    m = glm::translate(baseModel, glm::vec3(1.5f, 1.8f, 15.5f));
    m = glm::scale(m, glm::vec3(1.0f, 3.6f, 0.3f));
    cube.draw(shader, m);

    // Far right wall segment: X = +4 to +8 (center +6, width 4)
    m = glm::translate(baseModel, glm::vec3(6.0f, 1.8f, 15.5f));
    m = glm::scale(m, glm::vec3(4.0f, 3.6f, 0.3f));
    cube.draw(shader, m);

    // Left side wall
    m = glm::translate(baseModel, glm::vec3(-8.0f, 3.5f, 5.0f));
    m = glm::scale(m, glm::vec3(0.3f, 7.0f, 21.0f));
    cube.draw(shader, m);

    // Right side wall
    m = glm::translate(baseModel, glm::vec3(8.0f, 3.5f, 5.0f));
    m = glm::scale(m, glm::vec3(0.3f, 7.0f, 21.0f));
    cube.draw(shader, m);

    // Front wall (dark charcoal, behind stage)
    cube.ambient = glm::vec3(0.06f, 0.06f, 0.06f);
    cube.diffuse = glm::vec3(0.12f, 0.12f, 0.12f);
    cube.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    m = glm::translate(baseModel, glm::vec3(0.0f, 3.5f, -5.2f));
    m = glm::scale(m, glm::vec3(16.0f, 7.0f, 0.3f));
    cube.draw(shader, m);
}

// ─── WALL PANELS (oak + charcoal pillars + horizontal band) ─────────────
void drawWallPanels(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    // Dark charcoal vertical pillars
    cube.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    cube.diffuse = glm::vec3(0.12f, 0.12f, 0.12f);
    cube.specular = glm::vec3(0.15f, 0.15f, 0.15f);
    cube.emissive = glm::vec3(0.0f);

    for (int i = 0; i < 5; i++) {
        float zPos = -1.0f + i * 4.0f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(-7.8f, 3.5f, zPos));
        m = glm::scale(m, glm::vec3(0.12f, 6.0f, 0.25f)); cube.draw(shader, m);
    }
    for (int i = 0; i < 5; i++) {
        float zPos = -1.0f + i * 4.0f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(7.8f, 3.5f, zPos));
        m = glm::scale(m, glm::vec3(0.12f, 6.0f, 0.25f)); cube.draw(shader, m);
    }
    // Back wall charcoal pillars - SKIP ones that block door openings
    // Only draw at X=-6 and X=+6 (far sides, away from doors)
    {
        glm::mat4 m = glm::translate(baseModel, glm::vec3(-6.0f, 3.5f, 15.4f));
        m = glm::scale(m, glm::vec3(0.25f, 6.0f, 0.12f)); cube.draw(shader, m);
    }
    {
        glm::mat4 m = glm::translate(baseModel, glm::vec3(6.0f, 3.5f, 15.4f));
        m = glm::scale(m, glm::vec3(0.25f, 6.0f, 0.12f)); cube.draw(shader, m);
    }

    // Light oak wood panels
    cube.ambient = glm::vec3(0.4f, 0.32f, 0.18f);
    cube.diffuse = glm::vec3(0.72f, 0.58f, 0.35f);
    cube.specular = glm::vec3(0.25f, 0.2f, 0.12f);

    for (int i = 0; i < 4; i++) {
        float zPos = 1.0f + i * 4.0f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(-7.78f, 4.8f, zPos));
        m = glm::scale(m, glm::vec3(0.1f, 2.8f, 3.5f)); cube.draw(shader, m);
        m = glm::translate(baseModel, glm::vec3(-7.78f, 1.8f, zPos));
        m = glm::scale(m, glm::vec3(0.1f, 2.8f, 3.5f)); cube.draw(shader, m);
    }
    for (int i = 0; i < 4; i++) {
        float zPos = 1.0f + i * 4.0f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(7.78f, 4.8f, zPos));
        m = glm::scale(m, glm::vec3(0.1f, 2.8f, 3.5f)); cube.draw(shader, m);
        m = glm::translate(baseModel, glm::vec3(7.78f, 1.8f, zPos));
        m = glm::scale(m, glm::vec3(0.1f, 2.8f, 3.5f)); cube.draw(shader, m);
    }

    // Back wall oak panels - ONLY draw ABOVE door height (Y > 3.6) and at sides
    // Upper panels only (above doors) at Y=4.8
    for (int i = -1; i <= 1; i++) {
        glm::mat4 m = glm::translate(baseModel, glm::vec3(i * 4.0f, 5.5f, 15.38f));
        m = glm::scale(m, glm::vec3(3.5f, 2.0f, 0.08f)); cube.draw(shader, m);
    }
    // Lower panels only at FAR LEFT and FAR RIGHT (not covering doors)
    // Left panel at X=-4, only the left portion (X=-5.75 to X=-4)
    {
        glm::mat4 m = glm::translate(baseModel, glm::vec3(-6.0f, 1.8f, 15.38f));
        m = glm::scale(m, glm::vec3(2.5f, 2.8f, 0.08f)); cube.draw(shader, m);
    }
    // Right panel at X=+4, only the right portion
    {
        glm::mat4 m = glm::translate(baseModel, glm::vec3(6.0f, 1.8f, 15.38f));
        m = glm::scale(m, glm::vec3(2.5f, 2.8f, 0.08f)); cube.draw(shader, m);
    }

    // Horizontal dark trim bands
    cube.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    cube.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    cube.specular = glm::vec3(0.12f, 0.12f, 0.12f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(-7.8f, 3.3f, 7.0f));
    m = glm::scale(m, glm::vec3(0.12f, 0.15f, 18.0f)); cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(7.8f, 3.3f, 7.0f));
    m = glm::scale(m, glm::vec3(0.12f, 0.15f, 18.0f)); cube.draw(shader, m);

    // Back wall horizontal band - SPLIT to skip door openings
    // Left section: X=-8 to X=-4
    m = glm::translate(baseModel, glm::vec3(-6.0f, 3.3f, 15.4f));
    m = glm::scale(m, glm::vec3(4.0f, 0.15f, 0.12f)); cube.draw(shader, m);
    // Right section: X=+4 to X=+8
    m = glm::translate(baseModel, glm::vec3(6.0f, 3.3f, 15.4f));
    m = glm::scale(m, glm::vec3(4.0f, 0.15f, 0.12f)); cube.draw(shader, m);
    // Upper horizontal band (above doors, full width)
    m = glm::translate(baseModel, glm::vec3(0.0f, 5.0f, 15.4f));
    m = glm::scale(m, glm::vec3(16.0f, 0.15f, 0.12f)); cube.draw(shader, m);
}

// ─── CEILING (dark with wood grid) ──────────────────────────────────────
void drawCeiling(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = glm::vec3(0.08f, 0.07f, 0.06f);
    cube.diffuse = glm::vec3(0.15f, 0.13f, 0.11f);
    cube.specular = glm::vec3(0.1f, 0.1f, 0.08f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 7.0f, 0.0f));
    m = glm::scale(m, glm::vec3(16.0f, 0.2f, 10.0f)); cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(0.0f, 7.5f, 10.0f));
    m = glm::scale(m, glm::vec3(16.0f, 0.2f, 6.0f)); cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(0.0f, 7.25f, 5.0f));
    m = glm::rotate(m, glm::radians(2.5f), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::scale(m, glm::vec3(16.0f, 0.2f, 2.0f)); cube.draw(shader, m);

    // Grid lines (wood accent)
    cube.ambient = glm::vec3(0.12f, 0.09f, 0.06f);
    cube.diffuse = glm::vec3(0.22f, 0.17f, 0.1f);
    for (int i = -3; i <= 3; i++) {
        m = glm::translate(baseModel, glm::vec3(i * 2.0f, 6.92f, 3.0f));
        m = glm::scale(m, glm::vec3(0.04f, 0.04f, 16.0f)); cube.draw(shader, m);
    }
    for (int i = 0; i < 8; i++) {
        float zz = -2.0f + i * 2.2f;
        m = glm::translate(baseModel, glm::vec3(0.0f, 6.92f, zz));
        m = glm::scale(m, glm::vec3(16.0f, 0.04f, 0.04f)); cube.draw(shader, m);
    }
}

// ─── ROPE BARRIER (stanchion) ───────────────────────────────────────────
void drawRopeBarrier(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel, float x, float y, float z)
{
    cube.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    cube.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    cube.specular = glm::vec3(0.9f, 0.9f, 0.9f);
    cube.emissive = glm::vec3(0.0f);

    glm::mat4 m = glm::translate(baseModel, glm::vec3(x, y + 0.4f, z));
    m = glm::scale(m, glm::vec3(0.05f, 0.8f, 0.05f)); cube.draw(shader, m);
    cube.ambient = glm::vec3(0.25f, 0.25f, 0.25f);
    cube.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    m = glm::translate(baseModel, glm::vec3(x, y + 0.85f, z));
    m = glm::scale(m, glm::vec3(0.08f, 0.08f, 0.08f)); cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(x, y + 0.02f, z));
    m = glm::scale(m, glm::vec3(0.15f, 0.04f, 0.15f)); cube.draw(shader, m);
}

void drawRopeBarrierLine(Cube& cube, Cylinder& cylinder, Shader& shader, glm::mat4 baseModel)
{
    for (int i = 0; i < 8; i++) {
        float z = 1.0f + i * 1.75f; float y = i * 0.25f;
        drawRopeBarrier(cube, cylinder, shader, baseModel, -0.35f, y, z);
        drawRopeBarrier(cube, cylinder, shader, baseModel, 0.5f, y, z);
    }

    cube.ambient = glm::vec3(0.4f, 0.02f, 0.02f);
    cube.diffuse = glm::vec3(0.7f, 0.05f, 0.05f);
    cube.specular = glm::vec3(0.3f, 0.1f, 0.1f);
    cube.emissive = glm::vec3(0.0f);

    for (int i = 0; i < 7; i++) {
        float z1 = 1.0f + i * 1.75f; float z2 = 1.0f + (i + 1) * 1.75f;
        float y1 = i * 0.25f + 0.75f; float y2 = (i + 1) * 0.25f + 0.75f;
        float midZ = (z1 + z2) / 2.0f; float midY = (y1 + y2) / 2.0f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(-0.35f, midY, midZ));
        m = glm::scale(m, glm::vec3(0.025f, 0.025f, 1.75f)); cube.draw(shader, m);
        m = glm::translate(baseModel, glm::vec3(0.5f, midY, midZ));
        m = glm::scale(m, glm::vec3(0.025f, 0.025f, 1.75f)); cube.draw(shader, m);
    }
}

// ─── CEILING LIGHTS ─────────────────────────────────────────────────────
void drawCeilingLights(Sphere& lightSphere, Shader& shader, Shader& lightShader, glm::mat4 baseModel)
{
    lightSphere.ambient = glm::vec3(1.0f, 0.95f, 0.8f);
    lightSphere.diffuse = glm::vec3(1.0f, 0.95f, 0.8f);
    lightSphere.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    for (int ix = -2; ix <= 2; ix++) {
        for (int iz = 0; iz < 5; iz++) {
            float x = ix * 3.0f; float z = -1.0f + iz * 3.5f;
            float y = (iz < 3) ? 6.88f : 7.38f;
            glm::mat4 m = glm::translate(baseModel, glm::vec3(x, y, z));
            m = glm::scale(m, glm::vec3(0.12f, 0.06f, 0.12f));
            lightSphere.draw(shader, m);
        }
    }
}

// ─── TEXTURED OBJECTS (assignment) ──────────────────────────────────────
void drawTexturedObjects(Sphere& sphere1, Sphere& sphere2, Cone& cone1, Cone& cone2, Shader& shader, glm::mat4 baseModel)
{
    float time = (float)glfwGetTime();

    // Objects placed in BACK of audience area (NOT on stage)

}

// ═══════════════════════════════════════════════════════════════════════════
//  EXTERIOR BUILDING FUNCTIONS
//  Matching KUET / Bangladeshi university auditorium reference photos
//  - Off-white/light grey concrete body
//  - 6 triangular wedge buttress fins on front facade
//  - Upper protruding box with windows on left end
//  - Red brick side walls
//  - Decorative roofline blocks with cutouts
//  - Dark recessed door openings at ground level
//  - "AUDITORIUM" signage band
// ═══════════════════════════════════════════════════════════════════════════

// Color constants - OFF-WHITE concrete matching reference photo
static glm::vec3 CONCRETE_AMB(0.62f, 0.60f, 0.56f);
static glm::vec3 CONCRETE_DIF(0.92f, 0.90f, 0.85f);
static glm::vec3 CONCRETE_SPC(0.3f, 0.3f, 0.28f);

static glm::vec3 CONCRETE_DARK_AMB(0.52f, 0.50f, 0.47f);
static glm::vec3 CONCRETE_DARK_DIF(0.78f, 0.76f, 0.70f);

static glm::vec3 BRICK_AMB(0.38f, 0.14f, 0.06f);
static glm::vec3 BRICK_DIF(0.62f, 0.25f, 0.12f);
static glm::vec3 BRICK_SPC(0.1f, 0.06f, 0.04f);

static glm::vec3 DARK_AMB(0.04f, 0.04f, 0.05f);
static glm::vec3 DARK_DIF(0.08f, 0.08f, 0.1f);

// ─── MAIN EXTERIOR SHELL ────────────────────────────────────────────────
void drawExteriorBuilding(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);

    // Interior bounds: L=-8, R=8, Back=15.5, Front(stage)=-5.2
    // Building is ~16m wide, ~21m deep, ~8m tall to main roof

    // ═══ FRONT FACADE (Z = 16.0) ═══
    // Lower section: wall segments with 3 door openings matching interior
    // Doors at X = -3, 0, +3 (width 2 each)
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF; cube.specular = CONCRETE_SPC;

    // Upper band above doors (full width)
    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 5.3f, 16.3f));
    m = glm::scale(m, glm::vec3(17.5f, 3.4f, 0.6f));
    cube.draw(shader, m);

    // Far left (X = -8.75 to -4, center=-6.375, width=4.75)
    m = glm::translate(baseModel, glm::vec3(-6.375f, 1.8f, 16.3f));
    m = glm::scale(m, glm::vec3(4.75f, 3.6f, 0.6f));
    cube.draw(shader, m);

    // Pillar between door 1 and door 2 (X = -2 to -1, center=-1.5, width=1)
    m = glm::translate(baseModel, glm::vec3(-1.5f, 1.8f, 16.3f));
    m = glm::scale(m, glm::vec3(1.0f, 3.6f, 0.6f));
    cube.draw(shader, m);

    // Pillar between door 2 and door 3 (X = +1 to +2, center=+1.5, width=1)
    m = glm::translate(baseModel, glm::vec3(1.5f, 1.8f, 16.3f));
    m = glm::scale(m, glm::vec3(1.0f, 3.6f, 0.6f));
    cube.draw(shader, m);

    // Far right (X = +4 to +8.75, center=+6.375, width=4.75)
    m = glm::translate(baseModel, glm::vec3(6.375f, 1.8f, 16.3f));
    m = glm::scale(m, glm::vec3(4.75f, 3.6f, 0.6f));
    cube.draw(shader, m);

    // Front wall - upper portion (above buttresses, second floor)
    m = glm::translate(baseModel, glm::vec3(0.0f, 7.0f, 16.3f));
    m = glm::scale(m, glm::vec3(17.5f, 3.0f, 0.6f));
    cube.draw(shader, m);

    // ═══ SIDE WALLS ═══
    // Left exterior wall
    m = glm::translate(baseModel, glm::vec3(-8.75f, 4.5f, 5.0f));
    m = glm::scale(m, glm::vec3(0.5f, 9.0f, 23.0f));
    cube.draw(shader, m);

    // Right exterior wall
    m = glm::translate(baseModel, glm::vec3(8.75f, 4.5f, 5.0f));
    m = glm::scale(m, glm::vec3(0.5f, 9.0f, 23.0f));
    cube.draw(shader, m);

    // ═══ BACK WALL ═══
    m = glm::translate(baseModel, glm::vec3(0.0f, 4.5f, -6.5f));
    m = glm::scale(m, glm::vec3(18.0f, 9.0f, 0.5f));
    cube.draw(shader, m);

    // ═══ UPPER PROTRUDING BOX (left end, image 1) ═══
    // The left end has a box that juts forward with a large grille window
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;
    m = glm::translate(baseModel, glm::vec3(-7.5f, 6.5f, 17.5f));
    m = glm::scale(m, glm::vec3(3.5f, 4.0f, 2.5f));
    cube.draw(shader, m);

    // Grille window on protruding box
    cube.ambient = DARK_AMB; cube.diffuse = DARK_DIF;
    cube.specular = glm::vec3(0.15f, 0.15f, 0.15f);
    m = glm::translate(baseModel, glm::vec3(-7.5f, 6.8f, 18.8f));
    m = glm::scale(m, glm::vec3(2.2f, 2.0f, 0.1f));
    cube.draw(shader, m);

    // Grille bars (horizontal)
    cube.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
    cube.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
    for (int b = 0; b < 5; b++) {
        m = glm::translate(baseModel, glm::vec3(-7.5f, 6.1f + b * 0.4f, 18.85f));
        m = glm::scale(m, glm::vec3(2.0f, 0.04f, 0.04f));
        cube.draw(shader, m);
    }
    // Grille bars (vertical)
    for (int b = 0; b < 6; b++) {
        m = glm::translate(baseModel, glm::vec3(-8.3f + b * 0.4f, 6.8f, 18.85f));
        m = glm::scale(m, glm::vec3(0.04f, 2.0f, 0.04f));
        cube.draw(shader, m);
    }

    // ═══ HORIZONTAL CORNICE BANDS ═══
    cube.ambient = CONCRETE_DARK_AMB; cube.diffuse = CONCRETE_DARK_DIF;
    cube.specular = CONCRETE_SPC;

    // Band separating ground floor and buttress area
    m = glm::translate(baseModel, glm::vec3(0.0f, 4.15f, 16.65f));
    m = glm::scale(m, glm::vec3(18.0f, 0.15f, 0.8f));
    cube.draw(shader, m);

    // Band at top of buttresses / below upper floor
    m = glm::translate(baseModel, glm::vec3(0.0f, 5.5f, 16.65f));
    m = glm::scale(m, glm::vec3(18.0f, 0.15f, 0.8f));
    cube.draw(shader, m);

    // ═══ LOWER WOODEN WINDOWS (left end, image 1) ═══
    // Wooden window frames
    cube.ambient = glm::vec3(0.4f, 0.25f, 0.1f);
    cube.diffuse = glm::vec3(0.65f, 0.42f, 0.2f);
    cube.specular = glm::vec3(0.15f, 0.1f, 0.05f);

    for (int i = 0; i < 3; i++) {
        float wx = -7.8f + i * 1.5f;
        m = glm::translate(baseModel, glm::vec3(wx, 2.2f, 16.65f));
        m = glm::scale(m, glm::vec3(1.1f, 1.3f, 0.15f));
        cube.draw(shader, m);

        // Dark glass inside
        cube.ambient = DARK_AMB; cube.diffuse = DARK_DIF;
        m = glm::translate(baseModel, glm::vec3(wx, 2.2f, 16.72f));
        m = glm::scale(m, glm::vec3(0.9f, 1.1f, 0.05f));
        cube.draw(shader, m);
        cube.ambient = glm::vec3(0.4f, 0.25f, 0.1f);
        cube.diffuse = glm::vec3(0.65f, 0.42f, 0.2f);
    }
}

// ─── GROUND / COURTYARD ─────────────────────────────────────────────────
void drawExteriorGround(Cube& grassCube, Shader& shader, glm::mat4 baseModel)
{
    grassCube.emissive = glm::vec3(0.0f);

    // ═══ CONCRETE PAVEMENT (in front of building, Z > 16.3) ═══
    grassCube.ambient = glm::vec3(0.38f, 0.37f, 0.35f);
    grassCube.diffuse = glm::vec3(0.68f, 0.66f, 0.62f);
    grassCube.specular = glm::vec3(0.12f, 0.12f, 0.1f);

    // Main front courtyard (only in FRONT of building, Z = 17 to 37)
    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, -0.08f, 27.0f));
    m = glm::scale(m, glm::vec3(50.0f, 0.08f, 20.0f));
    grassCube.draw(shader, m);

    // ═══ ENTRANCE THRESHOLD FLOOR (fills gap between interior floor and exterior) ═══
    // This connects the interior floor (Z≈15.5, Y≈0) to the exterior door (Z≈16.6)
    grassCube.ambient = glm::vec3(0.35f, 0.33f, 0.3f);
    grassCube.diffuse = glm::vec3(0.6f, 0.55f, 0.5f);
    m = glm::translate(baseModel, glm::vec3(0.0f, -0.02f, 16.0f));
    m = glm::scale(m, glm::vec3(16.0f, 0.04f, 1.5f));
    grassCube.draw(shader, m);

    // ═══ GRASS AREAS (ONLY outside building footprint) ═══
    // Building footprint: X = -9 to +9, Z = -6.5 to +16.5
    // Grass goes ONLY outside these bounds
    grassCube.ambient = glm::vec3(0.5f, 0.6f, 0.35f);
    grassCube.diffuse = glm::vec3(0.7f, 0.85f, 0.5f);
    grassCube.specular = glm::vec3(0.15f, 0.2f, 0.1f);

    // Left grass (X = -24 to -9, full Z range outside building)
    m = glm::translate(baseModel, glm::vec3(-16.5f, -0.06f, 5.0f));
    m = glm::scale(m, glm::vec3(15.0f, 0.06f, 28.0f));
    grassCube.draw(shader, m);

    // Right grass (X = +9 to +24)
    m = glm::translate(baseModel, glm::vec3(16.5f, -0.06f, 5.0f));
    m = glm::scale(m, glm::vec3(15.0f, 0.06f, 28.0f));
    grassCube.draw(shader, m);

    // Back grass (behind building, Z = -22 to -6.5)
    m = glm::translate(baseModel, glm::vec3(0.0f, -0.06f, -14.0f));
    m = glm::scale(m, glm::vec3(35.0f, 0.06f, 14.0f));
    grassCube.draw(shader, m);

    // Front grass patches (in front, between courtyard and sides)
    // Left front grass
    m = glm::translate(baseModel, glm::vec3(-14.0f, -0.06f, 22.0f));
    m = glm::scale(m, glm::vec3(10.0f, 0.06f, 12.0f));
    grassCube.draw(shader, m);

    // Right front grass  
    m = glm::translate(baseModel, glm::vec3(14.0f, -0.06f, 22.0f));
    m = glm::scale(m, glm::vec3(10.0f, 0.06f, 12.0f));
    grassCube.draw(shader, m);

    // ═══ RED BRICK TILES (front courtyard approach) ═══
    grassCube.ambient = BRICK_AMB; grassCube.diffuse = BRICK_DIF; grassCube.specular = BRICK_SPC;
    m = glm::translate(baseModel, glm::vec3(0.0f, -0.03f, 24.0f));
    m = glm::scale(m, glm::vec3(20.0f, 0.06f, 8.0f));
    grassCube.draw(shader, m);
}

// ─── TRIANGULAR WEDGE BUTTRESS FINS (the key feature) ───────────────────
// From image 2: 6 large triangular/V-shaped concrete fins protrude from
// the front wall. Each is thick at the wall and tapers to a thin edge.
// They create deep triangular shadow pockets between them.
void drawZigzagFins(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF; cube.specular = CONCRETE_SPC;

    int numFins = 6;
    float startX = -5.5f;
    float spacing = 2.2f;
    float wallZ = 16.6f;
    float finProtrusion = 1.8f;  // how far forward from wall
    float finHeight = 4.0f;
    float baseY = 4.2f;         // bottom of fin area

    for (int i = 0; i < numFins; i++)
    {
        float cx = startX + i * spacing;

        // Each buttress is a triangular wedge:
        // Wide at the wall (left side), tapering to a thin edge (right side)
        // We approximate with 2 angled slabs forming a V/triangle shape

        // LEFT slab of the V (angled outward-left)
        glm::mat4 m = glm::translate(baseModel, glm::vec3(cx - 0.5f, baseY + finHeight * 0.5f, wallZ + finProtrusion * 0.5f));
        m = glm::rotate(m, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(0.18f, finHeight, finProtrusion * 0.9f));
        cube.draw(shader, m);

        // RIGHT slab of the V (angled outward-right)
        m = glm::translate(baseModel, glm::vec3(cx + 0.5f, baseY + finHeight * 0.5f, wallZ + finProtrusion * 0.5f));
        m = glm::rotate(m, glm::radians(-20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(0.18f, finHeight, finProtrusion * 0.9f));
        cube.draw(shader, m);

        // BOTTOM horizontal slab connecting the V at the base
        m = glm::translate(baseModel, glm::vec3(cx, baseY + 0.1f, wallZ + finProtrusion * 0.4f));
        m = glm::scale(m, glm::vec3(1.3f, 0.15f, finProtrusion * 0.7f));
        cube.draw(shader, m);

        // TOP horizontal slab (cap of the fin)
        cube.ambient = CONCRETE_DARK_AMB; cube.diffuse = CONCRETE_DARK_DIF;
        m = glm::translate(baseModel, glm::vec3(cx, baseY + finHeight, wallZ + finProtrusion * 0.3f));
        m = glm::scale(m, glm::vec3(1.5f, 0.2f, finProtrusion * 0.5f));
        cube.draw(shader, m);
        cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;

        // (No shadow pocket - door openings are real gaps in the wall)
        cube.specular = CONCRETE_SPC;
    }

    // Vertical pilaster between each fin pair (thin concrete dividers)
    for (int i = 0; i <= numFins; i++) {
        float px = startX - spacing * 0.5f + i * spacing;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(px, 5.0f, 16.65f));
        m = glm::scale(m, glm::vec3(0.15f, 6.0f, 0.3f));
        cube.draw(shader, m);
    }
}

// ─── EXTERIOR WINDOWS (upper floor rectangular openings) ────────────────
void drawExteriorWindows(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);

    // Upper floor white rectangular window openings (image 2 top row)
    // These are simple bright rectangles on the upper wall
    cube.ambient = glm::vec3(0.55f, 0.55f, 0.55f);
    cube.diffuse = glm::vec3(0.88f, 0.88f, 0.88f);
    cube.specular = glm::vec3(0.2f, 0.2f, 0.2f);

    for (int i = 0; i < 5; i++) {
        float x = -4.4f + i * 2.2f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(x, 7.5f, 16.65f));
        m = glm::scale(m, glm::vec3(1.0f, 1.6f, 0.12f));
        cube.draw(shader, m);
    }

    // Dark glass inset
    cube.ambient = glm::vec3(0.06f, 0.08f, 0.12f);
    cube.diffuse = glm::vec3(0.12f, 0.16f, 0.24f);
    cube.specular = glm::vec3(0.5f, 0.5f, 0.6f);

    for (int i = 0; i < 5; i++) {
        float x = -4.4f + i * 2.2f;
        glm::mat4 m = glm::translate(baseModel, glm::vec3(x, 7.5f, 16.72f));
        m = glm::scale(m, glm::vec3(0.8f, 1.3f, 0.06f));
        cube.draw(shader, m);
    }

    // Side wall windows
    for (int side = 0; side < 2; side++) {
        float wx = (side == 0) ? -9.0f : 9.0f;
        for (int i = 0; i < 3; i++) {
            float wz = 3.0f + i * 5.0f;
            // White surround
            cube.ambient = glm::vec3(0.55f, 0.55f, 0.55f);
            cube.diffuse = glm::vec3(0.88f, 0.88f, 0.88f);
            glm::mat4 m = glm::translate(baseModel, glm::vec3(wx, 7.5f, wz));
            m = glm::scale(m, glm::vec3(0.12f, 1.4f, 1.2f));
            cube.draw(shader, m);

            // Dark glass
            cube.ambient = glm::vec3(0.06f, 0.08f, 0.12f);
            cube.diffuse = glm::vec3(0.12f, 0.16f, 0.24f);
            float off = (side == 0) ? -0.03f : 0.03f;
            m = glm::translate(baseModel, glm::vec3(wx + off, 7.5f, wz));
            m = glm::scale(m, glm::vec3(0.06f, 1.1f, 1.0f));
            cube.draw(shader, m);
        }
    }
}

// ─── RED BRICK ACCENT WALLS (both sides, image 2) ──────────────────────
void drawBrickAccentWalls(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.ambient = BRICK_AMB; cube.diffuse = BRICK_DIF; cube.specular = BRICK_SPC;
    cube.emissive = glm::vec3(0.0f);

    // Left brick wall (extending forward from building)
    glm::mat4 m = glm::translate(baseModel, glm::vec3(-10.0f, 3.5f, 20.0f));
    m = glm::scale(m, glm::vec3(2.5f, 7.0f, 6.0f));
    cube.draw(shader, m);

    // Right brick wall
    m = glm::translate(baseModel, glm::vec3(10.0f, 3.5f, 20.0f));
    m = glm::scale(m, glm::vec3(2.5f, 7.0f, 6.0f));
    cube.draw(shader, m);

    // Left brick return (wrapping onto side of building)
    m = glm::translate(baseModel, glm::vec3(-9.5f, 3.5f, 16.0f));
    m = glm::scale(m, glm::vec3(1.5f, 7.0f, 2.0f));
    cube.draw(shader, m);

    // Right brick return
    m = glm::translate(baseModel, glm::vec3(9.5f, 3.5f, 16.0f));
    m = glm::scale(m, glm::vec3(1.5f, 7.0f, 2.0f));
    cube.draw(shader, m);

    // Horizontal mortar lines for brick texture
    cube.ambient = glm::vec3(0.45f, 0.2f, 0.1f);
    cube.diffuse = glm::vec3(0.7f, 0.35f, 0.18f);
    for (int j = 0; j < 12; j++) {
        float y = 0.5f + j * 0.6f;
        m = glm::translate(baseModel, glm::vec3(-10.0f, y, 23.05f));
        m = glm::scale(m, glm::vec3(2.5f, 0.03f, 0.04f));
        cube.draw(shader, m);
        m = glm::translate(baseModel, glm::vec3(10.0f, y, 23.05f));
        m = glm::scale(m, glm::vec3(2.5f, 0.03f, 0.04f));
        cube.draw(shader, m);
    }
}

// ─── ENTRANCE with ANIMATED WOODEN DOORS ────────────────────────────────
void drawEntrance(Cube& cube, Shader& shader, glm::mat4 baseModel, float doorAngle)
{
    cube.emissive = glm::vec3(0.0f);

    // ═══ SIGNAGE BAND ═══
    cube.ambient = glm::vec3(0.03f, 0.03f, 0.04f);
    cube.diffuse = glm::vec3(0.06f, 0.06f, 0.07f);
    cube.specular = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 4.0f, 16.72f));
    m = glm::scale(m, glm::vec3(12.0f, 0.5f, 0.15f));
    cube.draw(shader, m);

    // White text blocks
    cube.ambient = glm::vec3(0.6f, 0.6f, 0.6f);
    cube.diffuse = glm::vec3(0.92f, 0.92f, 0.92f);
    cube.specular = glm::vec3(0.2f, 0.2f, 0.2f);
    m = glm::translate(baseModel, glm::vec3(-2.0f, 4.0f, 16.8f));
    m = glm::scale(m, glm::vec3(3.0f, 0.3f, 0.06f));
    cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(2.0f, 4.0f, 16.8f));
    m = glm::scale(m, glm::vec3(3.5f, 0.3f, 0.06f));
    cube.draw(shader, m);

    // ═══ DOOR OPENINGS (gaps in wall provide the opening - no blocking geometry) ═══
    // Door frames only (thin trim around the gap, not blocking)

    // ═══ WOODEN DOORS (3 double-doors, rich dark brown wood) ═══
    cube.ambient = glm::vec3(0.22f, 0.12f, 0.04f);
    cube.diffuse = glm::vec3(0.45f, 0.25f, 0.1f);
    cube.specular = glm::vec3(0.15f, 0.1f, 0.05f);

    float doorWidth = 0.85f;
    float doorHeight = 3.4f;
    float doorThickness = 0.08f;
    float doorBottom = 0.05f;

    for (int i = -1; i <= 1; i++)
    {
        float doorCenterX = i * 3.0f;
        float doorZ = 16.62f;

        // LEFT door panel (hinged on left edge, swings inward = negative Z)
        glm::mat4 leftDoor = glm::translate(baseModel, glm::vec3(doorCenterX - doorWidth * 0.5f, doorBottom, doorZ));
        // Pivot on left edge: translate to hinge, rotate, translate back
        leftDoor = glm::translate(leftDoor, glm::vec3(-doorWidth * 0.5f, 0.0f, 0.0f));
        leftDoor = glm::rotate(leftDoor, glm::radians(doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        leftDoor = glm::translate(leftDoor, glm::vec3(doorWidth * 0.5f, 0.0f, 0.0f));
        leftDoor = glm::scale(leftDoor, glm::vec3(doorWidth, doorHeight, doorThickness));
        // Move origin to center of door panel for drawing
        glm::mat4 leftPanel = glm::translate(leftDoor, glm::vec3(0.0f, 0.5f, 0.0f));
        cube.draw(shader, leftPanel);

        // RIGHT door panel (hinged on right edge, swings inward = negative Z)
        glm::mat4 rightDoor = glm::translate(baseModel, glm::vec3(doorCenterX + doorWidth * 0.5f, doorBottom, doorZ));
        rightDoor = glm::translate(rightDoor, glm::vec3(doorWidth * 0.5f, 0.0f, 0.0f));
        rightDoor = glm::rotate(rightDoor, glm::radians(-doorAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        rightDoor = glm::translate(rightDoor, glm::vec3(-doorWidth * 0.5f, 0.0f, 0.0f));
        rightDoor = glm::scale(rightDoor, glm::vec3(doorWidth, doorHeight, doorThickness));
        glm::mat4 rightPanel = glm::translate(rightDoor, glm::vec3(0.0f, 0.5f, 0.0f));
        cube.draw(shader, rightPanel);

        // Door frame surround (wooden trim)
        cube.ambient = glm::vec3(0.2f, 0.12f, 0.04f);
        cube.diffuse = glm::vec3(0.4f, 0.25f, 0.1f);

        // Top frame
        m = glm::translate(baseModel, glm::vec3(doorCenterX, doorBottom + doorHeight + 0.05f, doorZ));
        m = glm::scale(m, glm::vec3(doorWidth * 2.0f + 0.15f, 0.1f, 0.12f));
        cube.draw(shader, m);
        // Left frame
        m = glm::translate(baseModel, glm::vec3(doorCenterX - doorWidth - 0.03f, doorBottom + doorHeight * 0.5f, doorZ));
        m = glm::scale(m, glm::vec3(0.06f, doorHeight + 0.1f, 0.12f));
        cube.draw(shader, m);
        // Right frame
        m = glm::translate(baseModel, glm::vec3(doorCenterX + doorWidth + 0.03f, doorBottom + doorHeight * 0.5f, doorZ));
        m = glm::scale(m, glm::vec3(0.06f, doorHeight + 0.1f, 0.12f));
        cube.draw(shader, m);

        // Door handle (small metallic knobs)
        cube.ambient = glm::vec3(0.15f, 0.15f, 0.15f);
        cube.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        cube.specular = glm::vec3(0.8f, 0.8f, 0.8f);
        // Only draw handles if door is mostly closed
        if (doorAngle < 30.0f) {
            m = glm::translate(baseModel, glm::vec3(doorCenterX - 0.15f, 1.8f, 16.7f));
            m = glm::scale(m, glm::vec3(0.04f, 0.12f, 0.06f));
            cube.draw(shader, m);
            m = glm::translate(baseModel, glm::vec3(doorCenterX + 0.15f, 1.8f, 16.7f));
            m = glm::scale(m, glm::vec3(0.04f, 0.12f, 0.06f));
            cube.draw(shader, m);
        }

        // Reset to wood color for next door
        cube.ambient = glm::vec3(0.28f, 0.16f, 0.06f);
        cube.diffuse = glm::vec3(0.52f, 0.32f, 0.14f);
        cube.specular = glm::vec3(0.18f, 0.12f, 0.06f);
    }

    // ═══ ENTRANCE STEPS ═══
    cube.ambient = CONCRETE_DARK_AMB; cube.diffuse = CONCRETE_DARK_DIF;
    cube.specular = CONCRETE_SPC;
    for (int s = 0; s < 3; s++) {
        m = glm::translate(baseModel, glm::vec3(0.0f, s * 0.1f, 17.0f + s * 0.35f));
        m = glm::scale(m, glm::vec3(10.0f, 0.1f, 0.35f));
        cube.draw(shader, m);
    }

    // ═══ ENTRANCE CANOPY ═══
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;
    m = glm::translate(baseModel, glm::vec3(0.0f, 3.6f, 17.2f));
    m = glm::scale(m, glm::vec3(10.0f, 0.15f, 1.5f));
    cube.draw(shader, m);
}

// ─── ROOF STRUCTURE (flat roof, parapet with decorative blocks) ─────────
void drawRoofStructure(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);

    // Flat roof slab
    cube.ambient = CONCRETE_DARK_AMB; cube.diffuse = CONCRETE_DARK_DIF;
    cube.specular = CONCRETE_SPC;

    glm::mat4 m = glm::translate(baseModel, glm::vec3(0.0f, 9.0f, 5.0f));
    m = glm::scale(m, glm::vec3(18.5f, 0.25f, 24.0f));
    cube.draw(shader, m);

    // ═══ DECORATIVE ROOFLINE BLOCKS (image 1 top edge) ═══
    // Square concrete blocks with rectangular/square cutouts
    // These sit on top of the parapet, spaced evenly along the front
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;

    float roofY = 9.5f;
    int numBlocks = 8;
    float blockSpacing = 2.0f;
    float blockStartX = -7.0f;

    for (int i = 0; i < numBlocks; i++)
    {
        float bx = blockStartX + i * blockSpacing;

        // Outer block (square frame)
        m = glm::translate(baseModel, glm::vec3(bx, roofY + 0.6f, 16.5f));
        m = glm::scale(m, glm::vec3(1.2f, 1.2f, 0.4f));
        cube.draw(shader, m);

        // Inner cutout (dark void representing the hole/opening)
        cube.ambient = DARK_AMB; cube.diffuse = DARK_DIF;
        cube.specular = glm::vec3(0.05f);

        // Square cutout
        m = glm::translate(baseModel, glm::vec3(bx, roofY + 0.6f, 16.65f));
        m = glm::scale(m, glm::vec3(0.7f, 0.7f, 0.25f));
        cube.draw(shader, m);

        // Some blocks have a circular-ish shape (approximated with rotated square)
        if (i % 2 == 0) {
            cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;
            m = glm::translate(baseModel, glm::vec3(bx, roofY + 0.6f, 16.7f));
            m = glm::rotate(m, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            m = glm::scale(m, glm::vec3(0.4f, 0.4f, 0.12f));
            cube.draw(shader, m);
        }

        cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;
        cube.specular = CONCRETE_SPC;
    }

    // ═══ PARAPET WALLS (low wall around roof edge) ═══
    // Front parapet
    m = glm::translate(baseModel, glm::vec3(0.0f, 9.3f, 16.3f));
    m = glm::scale(m, glm::vec3(18.0f, 0.5f, 0.4f));
    cube.draw(shader, m);

    // Side parapets
    m = glm::translate(baseModel, glm::vec3(-8.75f, 9.3f, 5.0f));
    m = glm::scale(m, glm::vec3(0.4f, 0.5f, 23.0f));
    cube.draw(shader, m);
    m = glm::translate(baseModel, glm::vec3(8.75f, 9.3f, 5.0f));
    m = glm::scale(m, glm::vec3(0.4f, 0.5f, 23.0f));
    cube.draw(shader, m);

    // Back parapet
    m = glm::translate(baseModel, glm::vec3(0.0f, 9.3f, -6.5f));
    m = glm::scale(m, glm::vec3(18.0f, 0.5f, 0.4f));
    cube.draw(shader, m);

    // ═══ STAIRCASE TOWER (right end, slightly taller block) ═══
    cube.ambient = CONCRETE_AMB; cube.diffuse = CONCRETE_DIF;
    m = glm::translate(baseModel, glm::vec3(7.5f, 7.5f, 16.5f));
    m = glm::scale(m, glm::vec3(2.5f, 6.0f, 1.5f));
    cube.draw(shader, m);

    // Window on staircase tower
    cube.ambient = DARK_AMB; cube.diffuse = DARK_DIF;
    m = glm::translate(baseModel, glm::vec3(7.5f, 7.5f, 17.3f));
    m = glm::scale(m, glm::vec3(1.2f, 2.0f, 0.1f));
    cube.draw(shader, m);
}

// ═══════════════════════════════════════════════════════════════════════════
//  CONTROLS / VIEWPORT
// ═══════════════════════════════════════════════════════════════════════════
void printControls()
{
    cout << "\n" << endl;
    cout << "         AUDITORIUM SCENE - FULL CONTROLS" << endl;
    cout << "" << endl;
    cout << "\n INTERIOR CAMERA PRESETS (F1-F12)" << endl;
    cout << "  F1  - Stage View (Speaker's POV)" << endl;
    cout << "  F2  - Front Row Center" << endl;
    cout << "  F3  - Back Row Center" << endl;
    cout << "  F4  - Left Side View" << endl;
    cout << "  F5  - Right Side View" << endl;
    cout << "  F6  - Bird's Eye / Top-Down" << endl;
    cout << "  F7  - High Back / Balcony" << endl;
    cout << "  F8  - Audience Left Close-up" << endl;
    cout << "  F9  - Audience Right Close-up" << endl;
    cout << "  F10 - Podium Close-up" << endl;
    cout << "  F11 - Isometric Front-Left" << endl;
    cout << "  F12 - Isometric Back-Right" << endl;
    cout << "\n EXTERIOR CAMERA PRESETS (SHIFT + F1-F6)" << endl;
    cout << "  SHIFT+F1 - Exterior Front (courtyard view)" << endl;
    cout << "  SHIFT+F2 - Exterior Left Side" << endl;
    cout << "  SHIFT+F3 - Exterior Right Side" << endl;
    cout << "  SHIFT+F4 - Exterior Back (behind stage)" << endl;
    cout << "  SHIFT+F5 - Aerial / Drone View" << endl;
    cout << "  SHIFT+F6 - Entrance Close-up" << endl;
    cout << "\n DOOR CONTROL" << endl;
    cout << "  SPACE - Open / Close entrance doors (animated)" << endl;
    cout << "\n DOG ANIMATION" << endl;
    cout << "  TAB   - Start / Stop dogs walking" << endl;
    cout << "\n CAMERA FREE MOVEMENT" << endl;
    cout << "  W/S/A/D   - Move Forward/Back/Left/Right" << endl;
    cout << "  Q/E       - Move Up / Down" << endl;
    cout << "  H/F       - Pan eye X | T/G - Pan eye Z" << endl;
    cout << "  Arrows    - Pitch / Yaw | [ ] - Roll" << endl;
    cout << "  Scroll    - Zoom" << endl;
    cout << "\n LIGHTING " << endl;
    cout << "  1 - Directional | 2 - Point | 3 - Spot" << endl;
    cout << "  5 - Ambient | 6 - Diffuse | 7 - Specular" << endl;
    cout << "\n TEXTURE & VIEWPORT " << endl;
    cout << "  4 - Viewport Modes | 8 - Textured Objects" << endl;
    cout << "  9 - Cycle Texture Mode | 0 - Reset" << endl;
    cout << "\n TRANSFORM " << endl;
    cout << "  X/Y/Z - Rotate | R - Reverse | I/K/J/L/O/P - Translate" << endl;
    cout << "  C/V/B/N/M/U - Scale | ESC - Exit" << endl;
    cout << "\n" << endl;
}

// ═══════════════════════════════════════════════════════════════════════════
//  BEZIER CURVE WALKWAY
//  Curved path from courtyard to auditorium entrance doors
//  Uses quadratic Bezier: B(t) = (1-t)²P0 + 2(1-t)tP1 + t²P2
// ═══════════════════════════════════════════════════════════════════════════
void drawBezierWalkway(Cube& cube, Shader& shader, glm::mat4 baseModel)
{
    cube.emissive = glm::vec3(0.0f);

    // Walkway color (grey concrete / road surface)
    cube.ambient = glm::vec3(0.35f, 0.33f, 0.3f);
    cube.diffuse = glm::vec3(0.6f, 0.58f, 0.52f);
    cube.specular = glm::vec3(0.15f, 0.15f, 0.12f);

    // ═══ MAIN CURVED WALKWAY (Bezier from far courtyard to entrance) ═══
    // Control points for the main walkway curve:
    //   P0 = start point (far courtyard, center)
    //   P1 = control point (pulls the curve outward)
    //   P2 = end point (entrance doors)

    glm::vec3 P0(0.0f, 0.01f, 32.0f);     // far end of courtyard
    glm::vec3 P1(0.0f, 0.01f, 24.0f);     // control (straight approach)
    glm::vec3 P2(0.0f, 0.01f, 17.5f);     // at entrance steps

    float walkwayWidth = 3.0f;
    int segments = 30;

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        // Quadratic Bezier: B(t) = (1-t)²P0 + 2(1-t)tP1 + t²P2
        float x1 = (1 - t1) * (1 - t1) * P0.x + 2 * (1 - t1) * t1 * P1.x + t1 * t1 * P2.x;
        float z1 = (1 - t1) * (1 - t1) * P0.z + 2 * (1 - t1) * t1 * P1.z + t1 * t1 * P2.z;
        float x2 = (1 - t2) * (1 - t2) * P0.x + 2 * (1 - t2) * t2 * P1.x + t2 * t2 * P2.x;
        float z2 = (1 - t2) * (1 - t2) * P0.z + 2 * (1 - t2) * t2 * P1.z + t2 * t2 * P2.z;

        float midX = (x1 + x2) * 0.5f;
        float midZ = (z1 + z2) * 0.5f;
        float segLen = sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));

        glm::mat4 m = glm::translate(baseModel, glm::vec3(midX, 0.02f, midZ));
        m = glm::scale(m, glm::vec3(walkwayWidth, 0.04f, segLen + 0.05f));
        cube.draw(shader, m);
    }

    // ═══ LEFT CURVED BRANCH (Bezier curve going left) ═══
    glm::vec3 L0(0.0f, 0.01f, 26.0f);         // starts from main path
    glm::vec3 L1(-6.0f, 0.01f, 28.0f);        // curves left
    glm::vec3 L2(-14.0f, 0.01f, 26.0f);       // ends far left

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        float x1 = (1 - t1) * (1 - t1) * L0.x + 2 * (1 - t1) * t1 * L1.x + t1 * t1 * L2.x;
        float z1 = (1 - t1) * (1 - t1) * L0.z + 2 * (1 - t1) * t1 * L1.z + t1 * t1 * L2.z;
        float x2 = (1 - t2) * (1 - t2) * L0.x + 2 * (1 - t2) * t2 * L1.x + t2 * t2 * L2.x;
        float z2 = (1 - t2) * (1 - t2) * L0.z + 2 * (1 - t2) * t2 * L1.z + t2 * t2 * L2.z;

        float midX = (x1 + x2) * 0.5f;
        float midZ = (z1 + z2) * 0.5f;
        float segLen = sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
        float angle = atan2(x2 - x1, z2 - z1);

        glm::mat4 m = glm::translate(baseModel, glm::vec3(midX, 0.02f, midZ));
        m = glm::rotate(m, -angle, glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(2.0f, 0.04f, segLen + 0.05f));
        cube.draw(shader, m);
    }

    // ═══ RIGHT CURVED BRANCH (Bezier curve going right) ═══
    glm::vec3 R0(0.0f, 0.01f, 26.0f);         // starts from main path
    glm::vec3 R1(6.0f, 0.01f, 28.0f);         // curves right
    glm::vec3 R2(14.0f, 0.01f, 26.0f);        // ends far right

    for (int i = 0; i < segments; i++)
    {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        float x1 = (1 - t1) * (1 - t1) * R0.x + 2 * (1 - t1) * t1 * R1.x + t1 * t1 * R2.x;
        float z1 = (1 - t1) * (1 - t1) * R0.z + 2 * (1 - t1) * t1 * R1.z + t1 * t1 * R2.z;
        float x2 = (1 - t2) * (1 - t2) * R0.x + 2 * (1 - t2) * t2 * R1.x + t2 * t2 * R2.x;
        float z2 = (1 - t2) * (1 - t2) * R0.z + 2 * (1 - t2) * t2 * R1.z + t2 * t2 * R2.z;

        float midX = (x1 + x2) * 0.5f;
        float midZ = (z1 + z2) * 0.5f;
        float segLen = sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
        float angle = atan2(x2 - x1, z2 - z1);

        glm::mat4 m = glm::translate(baseModel, glm::vec3(midX, 0.02f, midZ));
        m = glm::rotate(m, -angle, glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::scale(m, glm::vec3(2.0f, 0.04f, segLen + 0.05f));
        cube.draw(shader, m);
    }

    // ═══ WALKWAY EDGE BORDERS (thin dark strip along edges) ═══
    cube.ambient = glm::vec3(0.15f, 0.14f, 0.12f);
    cube.diffuse = glm::vec3(0.3f, 0.28f, 0.25f);

    // Main path borders
    for (int side = -1; side <= 1; side += 2) {
        for (int i = 0; i < segments; i++) {
            float t = ((float)i + 0.5f) / segments;
            float bx = (1 - t) * (1 - t) * P0.x + 2 * (1 - t) * t * P1.x + t * t * P2.x;
            float bz = (1 - t) * (1 - t) * P0.z + 2 * (1 - t) * t * P1.z + t * t * P2.z;

            glm::mat4 m = glm::translate(baseModel, glm::vec3(bx + side * walkwayWidth * 0.5f, 0.06f, bz));
            m = glm::scale(m, glm::vec3(0.1f, 0.08f, (P0.z - P2.z) / segments + 0.05f));
            cube.draw(shader, m);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  FRACTAL TREE
//  Recursive branching tree structure using cubes
//  Each branch splits into 2-3 smaller branches at an angle
// ═══════════════════════════════════════════════════════════════════════════
void drawFractalTree(Cube& cube, Cube& cubeLeaf, Shader& shader, glm::mat4 parentTransform,
    float length, float thickness, int depth, int maxDepth)
{
    if (depth > maxDepth || length < 0.05f) return;

    // ─── BRANCH ─────────────────────────────────────────────────────────
    if (depth <= maxDepth / 2) {
        // Trunk / main branches: brown wood
        cube.ambient = glm::vec3(0.18f, 0.1f, 0.04f);
        cube.diffuse = glm::vec3(0.35f, 0.2f, 0.08f);
        cube.specular = glm::vec3(0.1f, 0.08f, 0.04f);
    }
    else {
        // Upper branches: lighter brown
        cube.ambient = glm::vec3(0.22f, 0.14f, 0.06f);
        cube.diffuse = glm::vec3(0.42f, 0.28f, 0.12f);
        cube.specular = glm::vec3(0.1f, 0.08f, 0.04f);
    }
    cube.emissive = glm::vec3(0.0f);

    // Draw this branch as a tall thin cube
    glm::mat4 branchModel = glm::translate(parentTransform, glm::vec3(0.0f, length * 0.5f, 0.0f));
    branchModel = glm::scale(branchModel, glm::vec3(thickness, length, thickness));
    cube.draw(shader, branchModel);

    // ─── LEAVES (at the tips of branches) ───────────────────────────────
    if (depth >= maxDepth - 2) {
        cubeLeaf.ambient = glm::vec3(0.08f, 0.28f, 0.06f);
        cubeLeaf.diffuse = glm::vec3(0.15f, 0.5f, 0.12f);
        cubeLeaf.specular = glm::vec3(0.05f, 0.1f, 0.05f);
        cubeLeaf.emissive = glm::vec3(0.0f);

        glm::mat4 leafModel = glm::translate(parentTransform, glm::vec3(0.0f, length, 0.0f));
        leafModel = glm::scale(leafModel, glm::vec3(thickness * 6.0f, thickness * 3.0f, thickness * 6.0f));
        cubeLeaf.draw(shader, leafModel);
    }

    // ─── RECURSIVE BRANCHING ────────────────────────────────────────────
    glm::mat4 tipTransform = glm::translate(parentTransform, glm::vec3(0.0f, length, 0.0f));

    float childLength = length * 0.68f;
    float childThickness = thickness * 0.65f;
    float branchAngle = 25.0f + depth * 3.0f;  // wider angle as we go up

    // Branch 1: forward-left
    glm::mat4 branch1 = glm::rotate(tipTransform, glm::radians(branchAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    drawFractalTree(cube, cubeLeaf, shader, branch1, childLength, childThickness, depth + 1, maxDepth);

    // Branch 2: forward-right
    glm::mat4 branch2 = glm::rotate(tipTransform, glm::radians(-branchAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    drawFractalTree(cube, cubeLeaf, shader, branch2, childLength, childThickness, depth + 1, maxDepth);

    // Branch 3: forward (toward viewer) — adds depth
    glm::mat4 branch3 = glm::rotate(tipTransform, glm::radians(branchAngle * 0.8f), glm::vec3(1.0f, 0.0f, 0.0f));
    drawFractalTree(cube, cubeLeaf, shader, branch3, childLength * 0.85f, childThickness, depth + 1, maxDepth);

    // Branch 4: backward — makes tree fuller (only on lower branches)
    if (depth < maxDepth / 2) {
        glm::mat4 branch4 = glm::rotate(tipTransform, glm::radians(-branchAngle * 0.7f), glm::vec3(1.0f, 0.0f, 0.0f));
        drawFractalTree(cube, cubeLeaf, shader, branch4, childLength * 0.8f, childThickness, depth + 1, maxDepth);
    }
}

// ─── FULL TREE (trunk base + fractal branches) ─────────────────────────
void drawFractalTreeFull(Cube& cube, Cube& cubeLeaf, Shader& shader, glm::mat4 baseModel, float x, float z)
{
    // Position the tree at ground level
    glm::mat4 treeBase = glm::translate(baseModel, glm::vec3(x, 0.0f, z));

    // Tree parameters
    float trunkLength = 2.5f;
    float trunkThickness = 0.3f;
    int maxDepth = 5;  // 5 levels of branching (gives a nice full tree)

    // Draw the fractal tree starting from trunk
    drawFractalTree(cube, cubeLeaf, shader, treeBase, trunkLength, trunkThickness, 0, maxDepth);
}

// ═══════════════════════════════════════════════════════════════════════════
//  DOG (built from cubes with animated walking legs)
//  facing: -1 = walking toward building (negative Z), +1 = walking away
//  legAngle: oscillating angle for leg swing animation
// ═══════════════════════════════════════════════════════════════════════════
void drawDog(Cube& cube, Shader& shader, glm::mat4 baseModel, float x, float y, float z, float legAngle, int facing)
{
    cube.emissive = glm::vec3(0.0f);

    // Position and face the walking direction
    glm::mat4 dogBase = glm::translate(baseModel, glm::vec3(x, y, z));
    if (facing > 0)
        dogBase = glm::rotate(dogBase, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    float legSwing = sin(glm::radians(legAngle)) * 25.0f;  // ±25 degrees

    // ═══ BODY (main torso - brown/tan) ═══
    cube.ambient = glm::vec3(0.3f, 0.18f, 0.06f);
    cube.diffuse = glm::vec3(0.55f, 0.35f, 0.15f);
    cube.specular = glm::vec3(0.12f, 0.1f, 0.06f);

    glm::mat4 m = glm::translate(dogBase, glm::vec3(0.0f, 0.45f, 0.0f));
    m = glm::scale(m, glm::vec3(0.3f, 0.25f, 0.7f));
    cube.draw(shader, m);

    // ═══ HEAD ═══
    // Slightly lighter brown
    cube.ambient = glm::vec3(0.35f, 0.22f, 0.08f);
    cube.diffuse = glm::vec3(0.6f, 0.4f, 0.18f);

    m = glm::translate(dogBase, glm::vec3(0.0f, 0.55f, -0.4f));
    m = glm::scale(m, glm::vec3(0.22f, 0.22f, 0.22f));
    cube.draw(shader, m);

    // ═══ SNOUT (nose area) ═══
    cube.ambient = glm::vec3(0.32f, 0.2f, 0.07f);
    cube.diffuse = glm::vec3(0.55f, 0.38f, 0.16f);

    m = glm::translate(dogBase, glm::vec3(0.0f, 0.48f, -0.55f));
    m = glm::scale(m, glm::vec3(0.12f, 0.12f, 0.12f));
    cube.draw(shader, m);

    // ═══ NOSE (dark tip) ═══
    cube.ambient = glm::vec3(0.03f, 0.03f, 0.03f);
    cube.diffuse = glm::vec3(0.06f, 0.06f, 0.06f);

    m = glm::translate(dogBase, glm::vec3(0.0f, 0.5f, -0.62f));
    m = glm::scale(m, glm::vec3(0.05f, 0.04f, 0.03f));
    cube.draw(shader, m);

    // ═══ EYES (dark) ═══
    cube.ambient = glm::vec3(0.02f, 0.02f, 0.02f);
    cube.diffuse = glm::vec3(0.05f, 0.05f, 0.05f);
    cube.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    // Left eye
    m = glm::translate(dogBase, glm::vec3(-0.08f, 0.6f, -0.5f));
    m = glm::scale(m, glm::vec3(0.04f, 0.04f, 0.03f));
    cube.draw(shader, m);
    // Right eye
    m = glm::translate(dogBase, glm::vec3(0.08f, 0.6f, -0.5f));
    m = glm::scale(m, glm::vec3(0.04f, 0.04f, 0.03f));
    cube.draw(shader, m);

    // ═══ EARS (floppy, darker brown) ═══
    cube.ambient = glm::vec3(0.22f, 0.12f, 0.04f);
    cube.diffuse = glm::vec3(0.4f, 0.25f, 0.1f);
    cube.specular = glm::vec3(0.08f, 0.06f, 0.03f);

    // Left ear (drooping slightly)
    m = glm::translate(dogBase, glm::vec3(-0.12f, 0.62f, -0.35f));
    m = glm::rotate(m, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, glm::vec3(0.06f, 0.14f, 0.08f));
    cube.draw(shader, m);
    // Right ear
    m = glm::translate(dogBase, glm::vec3(0.12f, 0.62f, -0.35f));
    m = glm::rotate(m, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m = glm::scale(m, glm::vec3(0.06f, 0.14f, 0.08f));
    cube.draw(shader, m);

    // ═══ LEGS (animated swing when walking) ═══
    cube.ambient = glm::vec3(0.28f, 0.16f, 0.05f);
    cube.diffuse = glm::vec3(0.5f, 0.32f, 0.13f);
    cube.specular = glm::vec3(0.1f, 0.08f, 0.04f);

    float legLength = 0.3f;
    float legThick = 0.07f;

    // Front-left leg (swings forward when right swings back)
    m = glm::translate(dogBase, glm::vec3(-0.1f, 0.32f, -0.22f));
    m = glm::rotate(m, glm::radians(legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(legThick, legLength, legThick));
    cube.draw(shader, m);

    // Front-right leg (opposite phase)
    m = glm::translate(dogBase, glm::vec3(0.1f, 0.32f, -0.22f));
    m = glm::rotate(m, glm::radians(-legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(legThick, legLength, legThick));
    cube.draw(shader, m);

    // Back-left leg (same phase as front-right = diagonal gait)
    m = glm::translate(dogBase, glm::vec3(-0.1f, 0.32f, 0.22f));
    m = glm::rotate(m, glm::radians(-legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(legThick, legLength, legThick));
    cube.draw(shader, m);

    // Back-right leg (same phase as front-left)
    m = glm::translate(dogBase, glm::vec3(0.1f, 0.32f, 0.22f));
    m = glm::rotate(m, glm::radians(legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength * 0.5f, 0.0f));
    m = glm::scale(m, glm::vec3(legThick, legLength, legThick));
    cube.draw(shader, m);

    // ═══ PAWS (darker tips at bottom of legs) ═══
    cube.ambient = glm::vec3(0.15f, 0.08f, 0.03f);
    cube.diffuse = glm::vec3(0.3f, 0.18f, 0.08f);

    // Front-left paw
    m = glm::translate(dogBase, glm::vec3(-0.1f, 0.32f, -0.22f));
    m = glm::rotate(m, glm::radians(legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength, 0.0f));
    m = glm::scale(m, glm::vec3(0.08f, 0.04f, 0.1f));
    cube.draw(shader, m);

    // Front-right paw
    m = glm::translate(dogBase, glm::vec3(0.1f, 0.32f, -0.22f));
    m = glm::rotate(m, glm::radians(-legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength, 0.0f));
    m = glm::scale(m, glm::vec3(0.08f, 0.04f, 0.1f));
    cube.draw(shader, m);

    // Back-left paw
    m = glm::translate(dogBase, glm::vec3(-0.1f, 0.32f, 0.22f));
    m = glm::rotate(m, glm::radians(-legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength, 0.0f));
    m = glm::scale(m, glm::vec3(0.08f, 0.04f, 0.1f));
    cube.draw(shader, m);

    // Back-right paw
    m = glm::translate(dogBase, glm::vec3(0.1f, 0.32f, 0.22f));
    m = glm::rotate(m, glm::radians(legSwing), glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::translate(m, glm::vec3(0.0f, -legLength, 0.0f));
    m = glm::scale(m, glm::vec3(0.08f, 0.04f, 0.1f));
    cube.draw(shader, m);

    // ═══ TAIL (wagging when walking) ═══
    cube.ambient = glm::vec3(0.3f, 0.18f, 0.06f);
    cube.diffuse = glm::vec3(0.55f, 0.35f, 0.15f);

    float tailWag = dogsWalking ? sin(glm::radians(legAngle * 2.0f)) * 20.0f : 0.0f;

    m = glm::translate(dogBase, glm::vec3(0.0f, 0.55f, 0.35f));
    m = glm::rotate(m, glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // tail points up
    m = glm::rotate(m, glm::radians(tailWag), glm::vec3(0.0f, 0.0f, 1.0f));  // wag side to side
    m = glm::translate(m, glm::vec3(0.0f, 0.12f, 0.0f));
    m = glm::scale(m, glm::vec3(0.04f, 0.25f, 0.04f));
    cube.draw(shader, m);
}

void setViewport(int mode, int width, int height)
{
    switch (mode) {
    case 0: glViewport(0, 0, width, height); break;
    case 1: glViewport(0, height / 2, width / 2, height / 2); break;
    case 2: glViewport(width / 2, height / 2, width / 2, height / 2); break;
    case 3: glViewport(0, 0, width / 2, height / 2); break;
    case 4: glViewport(width / 2, 0, width / 2, height / 2); break;
    case 5: glViewport(0, height / 2, width / 2, height / 2); break;
    case 6: glViewport(width / 2, height / 2, width / 2, height / 2); break;
    case 7: glViewport(0, 0, width / 2, height / 2); break;
    case 8: glViewport(width / 2, 0, width / 2, height / 2); break;
    default: glViewport(0, 0, width, height); break;
    }
}