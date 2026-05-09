#pragma once

#include "TextElement.h"

#include <iostream>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

class Window {
public:
  static int init();
  static void mainLoop();
  static GLFWwindow* window;

  static double deltaTime;
  static int fbWidth, fbHeight;
private:
  static TextElement* fpsText;
};
