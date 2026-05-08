#include "Camera.h"

#include "Player.h"

glm::vec3 Camera::position(0.0f, 0.0f, 0.0f);
glm::vec3 Camera::rotation(0.0f, 0.0f, 0.0f);
glm::vec3 Camera::forward(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::right(1.0f, 0.0f, 0.0f);

float Camera::lastX = 400;
float Camera::lastY = 300;
float Camera::speed = 10.0f;
bool Camera::firstMouse = true;

void Camera::moveCamera(GLFWwindow* window) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  // yaw / pitch
  rotation.y += xoffset;
  rotation.x += yoffset;

  if (rotation.x > 89.0f) rotation.x = 89.0f;
  if (rotation.x < -89.0f) rotation.x = -89.0f;

  // forward vector
  glm::vec3 dir;
  dir.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
  dir.y = sin(glm::radians(rotation.x));
  dir.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

  forward = glm::normalize(dir);

  // 👉 compute right vector (THIS is what you were missing)
  right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

  // movement speed
  float velocity = speed * Window::deltaTime;

  if (!Player::currentPlayer) {
    // forward/back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= forward * velocity;

    // strafe left/right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
  } else {
    position = Player::currentPlayer->position + glm::vec3(0.0f, 1.0f, 0.0f); 
  }
}
