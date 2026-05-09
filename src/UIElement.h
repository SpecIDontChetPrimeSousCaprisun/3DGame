#pragma once

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <vector>

class UIElement {
public:
  UIElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, glm::vec3 color, int zIndex);
  UIElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, std::string texPath, int zIndex);

  static void initShader();
  static void drawAllElements();

  glm::vec3 color;
  glm::vec2 position, size;
  float backgroundTransparency, rotation;
  bool visible;
  int zIndex;
protected:
  virtual void afterDrawing();
  virtual ~UIElement() = default;
private:
  static std::map<int, std::vector<UIElement*>> elements;
  static unsigned int shaderProgram;

  bool hasTexture;
  unsigned int VAO, VBO, texture;

  void init();
  void draw();
};
