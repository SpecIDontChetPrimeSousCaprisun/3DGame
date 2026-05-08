#pragma once

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera {
public:
  static glm::vec3 position, rotation, forward, right;
  static float lastX;
  static float lastY;
  static float speed;
  static bool firstMouse;

  static void moveCamera(GLFWwindow* window);
};
