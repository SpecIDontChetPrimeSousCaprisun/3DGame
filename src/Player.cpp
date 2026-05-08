#include "Player.h"

#include "Camera.h"

Player* Player::currentPlayer = nullptr;
Player* Player::oldCurrentPlayer = nullptr;
bool changedPerspective = false;

Player::Player(glm::vec3 position, bool isCurrentPlayer) 
  : Mesh("models/Player.obj", glm::vec3(0.0f, 0.0f, 0.0f), position, glm::vec3(0.5f, 0.5f, 0.5f), false) { 
  if (isCurrentPlayer) {
    currentPlayer = this;
  }
}

void Player::updatePlayer(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !changedPerspective) {
    changedPerspective = true;

    if (currentPlayer) {
      oldCurrentPlayer = currentPlayer;
      currentPlayer = nullptr;

      return;
    } else if (oldCurrentPlayer) {
      currentPlayer = oldCurrentPlayer;
    } else {
      return;
    }
  } else if (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS) {
    changedPerspective = false;
  }

  if (!currentPlayer) return;

  float velocity = 10.0f * Window::deltaTime;

  glm::vec3 forward = Camera::forward;
  glm::vec3 right = Camera::right;

  forward.y = 0.0f;
  right.y = 0.0f;

  // forward/back
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      currentPlayer->position += forward * velocity;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      currentPlayer->position -= forward * velocity;

  // strafe left/right
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      currentPlayer->position += right * velocity;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      currentPlayer->position -= right * velocity;

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && currentPlayer->linearVelocity.y == 0)
      currentPlayer->linearVelocity.y += 15;
}
