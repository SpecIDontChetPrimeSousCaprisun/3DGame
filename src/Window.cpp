#include "Window.h"
#include "Mesh.h"
#include "Camera.h"
#include "Player.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// REQUIRED definition (missing in your case)
GLFWwindow* Window::window = nullptr;
bool spawned = false;

int Window::init() {
    glfwSetErrorCallback([](int error, const char* description)
    {
        std::cout << "GLFW ERROR: " << description << "\n";
    }); 

  /* Initialize the library */
    if (!glfwInit())
        return -1; 

    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    } 

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //glClearColor(0.2f, 0.4f, 0.6f, 1.0f);   

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glm::mat4 projection = glm::ortho(
        0.0f, 640.0f,
        480.0f, 0.0f,
        -1.0f, 1.0f
    );
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    return 0;
}

void Window::mainLoop() {
  while (!glfwWindowShouldClose(window)){
    glfwPollEvents();

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    glViewport(0, 0, fbWidth, fbHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !spawned) {
      spawned = true;
      new Mesh("models/Box.obj", "textures/Texture.jpg", glm::vec3(0.0f, 0.0f, 0.0f), Camera::position + (Camera::forward * 2.0f), false);
    } else if (glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS ) {
      spawned = false;
    }

    Mesh::calculateAllPhysics();
    Camera::moveCamera(window);
    Player::updatePlayer(window);
    Mesh::drawAllMeshes();

    glfwSwapBuffers(window);
  }

  glfwTerminate();
}
