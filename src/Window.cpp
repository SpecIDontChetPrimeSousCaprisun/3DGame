#include "Window.h"
#include "Mesh.h"
#include "Camera.h"
#include "Player.h"
#include "UIElement.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iomanip>

// REQUIRED definition (missing in your case)
GLFWwindow* Window::window = nullptr;
double Window::deltaTime = 0;
int Window::fbWidth = 600;
int Window::fbHeight = 480;

double lastFrame = glfwGetTime();
bool spawned = false;
bool toggledLock = false;
bool locked = true;
bool resizing = false;
TextElement* Window::fpsText = nullptr;

int Window::init() {
  glfwSetErrorCallback([](int error, const char* description) {
      std::cout << "GLFW ERROR: " << description << "\n";
  }); 

/* Initialize the library */
  if (!glfwInit())
      return -1; 

  glfwWindowHint(GLFW_DEPTH_BITS, 24);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(fbWidth, fbHeight, "Game", NULL, NULL);

  if (!window)
  {
      glfwTerminate();
      return -1;
  } 

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0); 

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD\n";
      return -1;
  }

  glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glm::mat4 projection = glm::ortho(
      0.0f, (float)fbWidth,
      (float)fbHeight, 0.0f,
      -1.0f, 1.0f
  );
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

  glfwSetFramebufferSizeCallback(window,
  [](GLFWwindow*, int width, int height)
  {
      resizing = (width == 0 || height == 0);
  });

  return 0;
}

void Window::mainLoop() {
  fpsText =  new TextElement(glm::vec2(10.0f, 10.0f), glm::vec2(200.0f, 50.0f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "FPS : 60", "fonts/Kenney Future Narrow.ttf", 1);

  while (!glfwWindowShouldClose(window)){
    if (resizing) {
      glfwPollEvents();
      glfwSwapBuffers(window);
      return;
    }

    glfwPollEvents();

    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (deltaTime > 0.1) {
      deltaTime = 0.1;
    }

    double fps = 1.0 / deltaTime;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(1) << fps;
    std::string fpsString = ss.str();

    fpsText->text = "FPS : " + fpsString;

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    if (fbWidth == 0 || fbHeight == 0) {
      glfwSwapBuffers(window);
      continue;
    }

    glViewport(0, 0, fbWidth, fbHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !spawned) {
      spawned = true;
      new Mesh("models/Box.obj", "textures/Texture.jpg", glm::vec3(0.0f, 0.0f, 0.0f), Camera::position + (Camera::forward * 5.0f), false);
    } else if (glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS ) {
      spawned = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !toggledLock) {
      if (locked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        locked = false;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        locked = true;
      }

      toggledLock = true;
    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
      toggledLock = false;
    }

    Mesh::calculateAllPhysics();
    Camera::moveCamera(window);
    Player::updatePlayer(window);
    Mesh::drawAllShadows();
    Mesh::drawAllMeshes();
    UIElement::drawAllElements();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}
