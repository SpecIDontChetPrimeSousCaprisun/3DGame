#include "UIElement.h"
#include "FileLoader.h"

unsigned int UIElement::shaderProgram = 0;
std::vector<UIElement*> UIElement::elements;

void UIElement::drawAllElements() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  for (UIElement* element : elements) {
    element->draw();
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void UIElement::init() {
  float vertices[] = {
      // positions   // UVs
      0.0f, 0.0f,    0.0f, 0.0f,
      1.0f, 0.0f,    1.0f, 0.0f,
      1.0f, 1.0f,    1.0f, 1.0f,

      1.0f, 1.0f,    1.0f, 1.0f,
      0.0f, 1.0f,    0.0f, 1.0f,
      0.0f, 0.0f,    0.0f, 0.0f
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(vertices),
      vertices,
      GL_STATIC_DRAW
  );

  // position
  glVertexAttribPointer(
      0,
      2,
      GL_FLOAT,
      GL_FALSE,
      4 * sizeof(float),
      (void*)0
  );
  glEnableVertexAttribArray(0);

  // UV
  glVertexAttribPointer(
      1,
      2,
      GL_FLOAT,
      GL_FALSE,
      4 * sizeof(float),
      (void*)(2 * sizeof(float))
  );
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  visible = true;
  elements.push_back(this);
}

UIElement::UIElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, glm::vec3 color)
: position(position), size(size), backgroundTransparency(backgroundTransparency), color(color), rotation(0.0f), hasTexture(false) {
  init();
}

UIElement::UIElement(glm::vec2 position, glm::vec2 size, float backgroundTransparency, std::string texPath)
: position(position), size(size), backgroundTransparency(backgroundTransparency), texture(FileLoader::loadTexture(texPath)), rotation(0.0f), hasTexture(true) {
  init();
}

void UIElement::initShader() {
  // already initialized
  if (shaderProgram != 0)
      return;

  std::string vertexCode =
      FileLoader::loadFile("shaders/2DVertex.glsl");

  std::string fragmentCode =
      FileLoader::loadFile("shaders/2DFragment.glsl");

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

  std::cout << "2D shader initialized\n";
}

void UIElement::afterDrawing() {}

void UIElement::draw() {
  if (!visible) return;

  glUseProgram(shaderProgram);

  // ===== MODEL =====
  glm::mat4 model = glm::mat4(1.0f);

  model = glm::translate(
      model,
      glm::vec3(position, 0.0f)
  );

  // rotate around center
  model = glm::translate(
      model,
      glm::vec3(size * 0.5f, 0.0f)
  );

  model = glm::rotate(
      model,
      glm::radians(rotation),
      glm::vec3(0.0f, 0.0f, 1.0f)
  );

  model = glm::translate(
      model,
      glm::vec3(-size * 0.5f, 0.0f)
  );

  model = glm::scale(
      model,
      glm::vec3(size, 1.0f)
  );

  // ===== ORTHOGRAPHIC PROJECTION =====
  int width, height;
  glfwGetFramebufferSize(
      glfwGetCurrentContext(),
      &width,
      &height
  );

  glm::mat4 projection = glm::ortho(
      0.0f,
      (float)width,
      (float)height,
      0.0f,
      -1.0f,
      1.0f
  );

  // ===== SEND MATRICES =====
  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "model"),
      1,
      GL_FALSE,
      glm::value_ptr(model)
  );

  glUniformMatrix4fv(
      glGetUniformLocation(shaderProgram, "projection"),
      1,
      GL_FALSE,
      glm::value_ptr(projection)
  );

  // ===== COLOR =====
  glUniform3f(
      glGetUniformLocation(shaderProgram, "color"),
      color.r,
      color.g,
      color.b
  );

  glUniform1f(
      glGetUniformLocation(shaderProgram, "alpha"),
      backgroundTransparency
  );

  glUniform1i(
      glGetUniformLocation(shaderProgram, "hasTexture"),
      hasTexture
  );

  // ===== TEXTURE =====
  if (hasTexture) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture);

      glUniform1i(
          glGetUniformLocation(shaderProgram, "tex"),
          0
      );
  }

  // ===== DRAW =====
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);

  afterDrawing();
}
