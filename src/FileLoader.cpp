#include <stb/stb_image.h>
#include "FileLoader.h"

std::string FileLoader::loadFile(const char* path)
{
    std::ifstream file(path);
    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

unsigned int FileLoader::loadTexture(const std::string& path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cout << "Failed to load texture: " << path << std::endl;
        return 0;
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

std::vector<unsigned char> FileLoader::loadFontFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file),
        {}
    );
}
