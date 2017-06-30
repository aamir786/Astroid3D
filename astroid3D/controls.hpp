//
//  controls.hpp
//  3d_Astroid
//

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getViewPos();
glm::vec3 getViewDirection();

#endif
