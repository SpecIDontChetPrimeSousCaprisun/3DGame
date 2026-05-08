#pragma once

#include "../include/glad/glad.h"

#include <string>
#include <vector>

#include <stb/stb_truetype.h>

class Font {
public:
    Font(const std::string& path, float pixelHeight);

    unsigned int texture;

    stbtt_bakedchar cdata[96];

private:
    std::vector<unsigned char> ttfBuffer;
};
