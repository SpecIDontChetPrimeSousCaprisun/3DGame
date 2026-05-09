#include "Light.h"

std::vector<Light*> Light::lights;

Light::Light(std::string path, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored, float intensity)
  : Mesh(path, angularVelocity, position, color, anchored), intensity(intensity), enabled(true) {
  lights.push_back(this);
};
