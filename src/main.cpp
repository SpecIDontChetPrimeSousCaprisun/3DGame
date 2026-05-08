#include "Window.h"
#include "Mesh.h"
#include "Player.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
  if (Window::init() == -1) return -1;
  Mesh::initShader();

  std::vector<float> vertices = Mesh::getVerticesFromFile("models/Baseplate.obj");

  new Player(glm::vec3(0.0f, 0.0f, 0.0f), true);
  new Mesh(vertices.data(), vertices.size() * sizeof(float), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), true); 
  new Mesh("models/Wall.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -5.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f), true);

  Window::mainLoop();
}
