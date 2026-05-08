#pragma once

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

class FileLoader {
public:
  static std::string loadFile(const char* path);
  static unsigned int loadTexture(const std::string& path);
  static std::vector<unsigned char> loadFontFile(const std::string& path);
};
