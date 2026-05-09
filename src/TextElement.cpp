#include "TextElement.h"
#include "FileLoader.h"
#include "Window.h"

unsigned int TextElement::shaderProgram = 0;

void TextElement::init() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

  // position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // uv
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

TextElement::TextElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, glm::vec3 color, glm::vec3 textColor, std::string text, std::string fontPath, float scale) 
  : UIElement(position, size, backgroundTransparency, color), text(text), font(new Font(fontPath, size.y)), scale(scale), textColor(textColor) {
  init();
}

TextElement::TextElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, std::string texPath, glm::vec3 textColor, std::string text, std::string fontPath, float scale)
  : UIElement(position, size, backgroundTransparency, texPath), text(text), font(new Font(fontPath, size.y)), scale(scale), textColor(textColor) {
  init();
}

void TextElement::initShader() {
  // already initialized
  if (shaderProgram != 0)
      return;

  std::string vertexCode =
      FileLoader::loadFile("shaders/textVertex.glsl");

  std::string fragmentCode =
      FileLoader::loadFile("shaders/textFragment.glsl");

  const char* vertexShaderSource = vertexCode.c_str();
  const char* fragmentShaderSource = fragmentCode.c_str();

  // ===== VERTEX SHADER =====
  unsigned int vertexShader =
      glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(
      vertexShader,
      1,
      &vertexShaderSource,
      NULL
  );

  glCompileShader(vertexShader);

  int success;
  char infoLog[512];

  glGetShaderiv(
      vertexShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          vertexShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "VERTEX SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== FRAGMENT SHADER =====
  unsigned int fragmentShader =
      glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(
      fragmentShader,
      1,
      &fragmentShaderSource,
      NULL
  );

  glCompileShader(fragmentShader);

  glGetShaderiv(
      fragmentShader,
      GL_COMPILE_STATUS,
      &success
  );

  if (!success) {
      glGetShaderInfoLog(
          fragmentShader,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "FRAGMENT SHADER ERROR:\n"
          << infoLog
          << std::endl;
  }

  // ===== SHADER PROGRAM =====
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);

  glGetProgramiv(
      shaderProgram,
      GL_LINK_STATUS,
      &success
  );

  if (!success) {
      glGetProgramInfoLog(
          shaderProgram,
          512,
          NULL,
          infoLog
      );

      std::cout
          << "PROGRAM LINK ERROR:\n"
          << infoLog
          << std::endl;
  }

  // cleanup
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  std::cout << "Text shader initialized\n";
}

void TextElement::afterDrawing() {
  glDisable(GL_DEPTH_TEST);

  glUseProgram(shaderProgram);

  glm::mat4 projection = glm::ortho(
      0.0f, (float)Window::fbWidth,
      (float)Window::fbHeight, 0.0f,
      -1.0f, 1.0f
  );

  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "projection"),
      1,
      GL_FALSE,
      glm::value_ptr(projection)
  );

  glUniform3f(
      glGetUniformLocation(shaderProgram, "color"),
      textColor.r, textColor.g, textColor.b
  );

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font->texture);

  glUniform1i(
      glGetUniformLocation(shaderProgram, "tex"),
      0
  );

  glBindVertexArray(VAO);

  float startX = position.x;
  float x = startX;
  float y = position.y + size.y;

  for (char c : text) {
      if (c < 32 || c >= 128)
          continue;

      stbtt_aligned_quad q;

      stbtt_GetBakedQuad(
          font->cdata,
          512, 512,
          c - 32,
          &x, &y,
          &q,
          1
      );

      float vertices[6][4] = {
          { q.x0, q.y0, q.s0, q.t0 },
          { q.x1, q.y0, q.s1, q.t0 },
          { q.x1, q.y1, q.s1, q.t1 },

          { q.x1, q.y1, q.s1, q.t1 },
          { q.x0, q.y1, q.s0, q.t1 },
          { q.x0, q.y0, q.s0, q.t0 }
      };

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

      glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  size.x = x - startX;

  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
}
