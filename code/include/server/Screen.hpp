#pragma once
#ifndef __NR_SCREEN_HPP__
#define __NR_SCREEN_HPP__

#include "geometry/vec.hpp"
#include "common/macros.hpp"
#include <mutex>

namespace NRenderer
{
    class DLL_EXPORT Screen
    {
    private:
        RGBA* pixels;
        unsigned int width;
        unsigned int height;
        mutable bool updated;
        mutable mutex mtx;
    public:
        Screen();
        Screen(const Screen&) = delete;
        ~Screen();
        void set(RGBA* pixels, int width, int height);
        unsigned int getWidth() const;
        unsigned int getHeight() const;
        const RGBA* getPixels() const;
        void release();
        bool isUpdated() const;
    };  
} // namespace NRenderer


#endif