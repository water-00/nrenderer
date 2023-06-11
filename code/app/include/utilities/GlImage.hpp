#pragma once
#ifndef __NR_GL_IMAGE_HPP__
#define __NR_GL_IMAGE_HPP__

#include "geometry/vec.hpp"
#include "glad/glad.h"

namespace NRenderer
{
    using GlImageId = unsigned int;
    class GlImage
    {
    private:
    public:
        static GlImageId loadImage(const RGBA* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }
        static void deleteImage(GlImageId id) {
            glDeleteTextures(1, &id);
        }
        static GlImageId loadImage(const RGB* pixels, const Vec2& size) {
            GlImageId id  = 0u;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGB, GL_FLOAT, pixels);
            glBindTexture(GL_TEXTURE_2D, 0);
            return id;
        }
    };    
} // namespace NRenderer


#endif