#pragma once

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

class FileLoader {
public:
  static std::string loadFile(const char* path);
  static unsigned int loadTexture(const std::string& path);
};
