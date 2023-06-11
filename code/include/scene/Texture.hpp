#pragma once
#ifndef __NR_TEXTURE_HPP__
#define __NR_TEXTURE_HPP__

#include <memory>

#include "geometry/vec.hpp"

namespace NRenderer
{
    using namespace std;
    struct Texture
    {
        Texture()
            : height(0)
            , width(0)
            , rgba(nullptr)
        {}
        ~Texture() {
            delete[]  rgba;
        }
        Texture(const Texture& texture) {
            this->height = texture.height;
            this->width = texture.width;
            this->rgba = new RGBA[texture.height*texture.width];
            for (int i = 0; i < texture.height*texture.width; i++) {
                this->rgba[i] = texture.rgba[i];
            }
        }
        Texture(Texture&& texture) noexcept {
            this->height = texture.height;
            this->width = texture.width;
            this->rgba = texture.rgba;
            texture.rgba = nullptr;
        }
        unsigned int height;
        unsigned int width;
        RGBA* rgba;
    };
    using SharedTexture = shared_ptr<Texture>;
}

#endif