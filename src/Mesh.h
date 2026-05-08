#pragma once

#include "FileLoader.h"
#include "Camera.h"
#include "Window.h"

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <cfloat>
#include <cmath>
#include <limits>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh {
public:
  Mesh(float* vertices, size_t length, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored);
  Mesh(std::string path, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored);

  static std::vector<float> getVerticesFromFile(std::string path);
  static void calculateAllPhysics();
  static void drawAllMeshes();
  static void initShader();

  glm::vec3 angularVelocity, linearVelocity, position, color;
  bool anchored;
private:
  static glm::vec3 resolveAABB(Mesh* a, Mesh* b);
  static std::vector<Mesh*> meshes;
  static unsigned int shaderProgram;

  glm::vec3 colliderSize, colliderMin, colliderMax;
  unsigned int VAO, VBO;
  int vertexCount;
  bool intersects(Mesh* other);
  void init(float* vertices, size_t length);
  void computeCollider(float* vertices, size_t size);
  void calculatePhysics();
  void draw();
};
