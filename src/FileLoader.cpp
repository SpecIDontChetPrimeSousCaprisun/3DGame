#include "FileLoader.h"

std::string FileLoader::loadFile(const char* path)
{
    std::ifstream file(path);
    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}
