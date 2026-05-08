#include "Font.h"
#include "FileLoader.h"

Font::Font(const std::string& path, float pixelHeight)
{
    ttfBuffer = FileLoader::loadFontFile(path);

    const int BITMAP_W = 512;
    const int BITMAP_H = 512;

    unsigned char bitmap[BITMAP_W * BITMAP_H];

    stbtt_BakeFontBitmap(
        ttfBuffer.data(),
        0,
        pixelHeight,
        bitmap,
        BITMAP_W,
        BITMAP_H,
        32,
        96,
        cdata
    );

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        BITMAP_W,
        BITMAP_H,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        bitmap
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}
