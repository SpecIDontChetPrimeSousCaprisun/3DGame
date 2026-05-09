#pragma once

#include "UIElement.h"
#include "Font.h"

#include <stb/stb_truetype.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>

class TextElement : public UIElement {
public:
  TextElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, glm::vec3 color, glm::vec3 textColor, std::string text, std::string fontPath, float scale, int zIndex);
  TextElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, std::string texPath, glm::vec3 textColor, std::string text, std::string fontPath, float scale, int zIndex);

  static void initShader();

  std::string text;
  Font* font;
  float scale;
  glm::vec3 textColor;
protected:
  void afterDrawing() override;
private:
  static unsigned int shaderProgram;

  unsigned int VAO, VBO;

  void init();
};
