#pragma once

#include "Mesh.h"

class Player : public Mesh {
public:
  Player(glm::vec3 position, bool isCurrentPlayer);

  static Player* currentPlayer;
  static Player* oldCurrentPlayer;
  static void updatePlayer(GLFWwindow* window);

  float health, maxHealth;
};
