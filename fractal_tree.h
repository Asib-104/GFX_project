#ifndef fractal_tree_h
#define fractal_tree_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cylinder.h"
#include "shader.h"

class FractalTree {
public:
    static void draw(Cylinder& branchCylinder, Cylinder& leafCylinder, Shader& shader, glm::mat4 model, int depth)
    {
        if (depth == 0) return;

        glm::mat4 branchModel = model;
        // The standard Cylinder class draws from y = -0.5 to y = 0.5 centered at origin.
        // We translate up by 0.5 so its base is at the current model origin.
        branchModel = glm::translate(branchModel, glm::vec3(0.0f, 0.5f, 0.0f));
        
        // Scale it: thickness depends on depth, height is 1.0 at this local level
        float thickness = 0.04f * depth; 
        branchModel = glm::scale(branchModel, glm::vec3(thickness, 1.0f, thickness));
        
        if (depth == 1 || depth == 2) { 
            // Draw leaves at the end
            leafCylinder.draw(shader, branchModel);
        } else {
            // Draw wooden trunk
            branchCylinder.draw(shader, branchModel);
        }

        // Calculate transformation for next branches
        // Move to the top of the current branch (local y=1.0)
        glm::mat4 topModel = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Decrease scale of branches (they get shorter)
        topModel = glm::scale(topModel, glm::vec3(0.8f, 0.8f, 0.8f));

        // Create right branch
        glm::mat4 rightModel = glm::rotate(topModel, glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        rightModel = glm::rotate(rightModel, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        draw(branchCylinder, leafCylinder, shader, rightModel, depth - 1);

        // Create left branch
        glm::mat4 leftModel = glm::rotate(topModel, glm::radians(-35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        leftModel = glm::rotate(leftModel, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        draw(branchCylinder, leafCylinder, shader, leftModel, depth - 1);
        
        // Create front/back branch to give it a 3D volume
        glm::mat4 frontModel = glm::rotate(topModel, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        frontModel = glm::rotate(frontModel, glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        draw(branchCylinder, leafCylinder, shader, frontModel, depth - 1);
    }
};

#endif /* fractal_tree_h */
