#pragma once

#include <fstream>
#include <sstream>
#include <string>

class FileLoader {
public:
  static std::string loadFile(const char* path);
};
