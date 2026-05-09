#pragma once

#include "Mesh.h"

#include <vector>

class Light : public Mesh {
public:
    Light(std::string path, glm::vec3 angularVelocity, glm::vec3 position, glm::vec3 color, bool anchored, float intensity);

    static std::vector<Light*> lights;

    bool enabled;
    float intensity;
};
