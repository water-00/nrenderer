#include "Server/Screen.hpp"

#include <cstdlib>

namespace NRenderer
{
    Screen::Screen()
        : width             (500)
        , height            (500)
        , updated           (false)
        , mtx               ()
    {
        pixels = new RGBA[height * width];
        for (int i=0; i<height; i++) {
            for (int j=0; j<width;j++) {
                pixels[i*width+j] = {0, 0, 0, 1};
            }
        }
    }
    bool Screen::isUpdated() const {
        return updated;
    }
    Screen::~Screen() {
        if (pixels!=nullptr) delete[] pixels;
    }
    void Screen::release() {
        mtx.lock();
        if (pixels == nullptr) {
            mtx.unlock();
            return;
        }
        delete[] pixels;
        pixels = nullptr;
        mtx.unlock();
    }
    unsigned int Screen::getWidth() const {
        mtx.lock();
        auto w = width;
        mtx.unlock();
        return w;
    }
    unsigned int Screen::getHeight() const {
        mtx.lock();
        int h = height;
        mtx.unlock();
        return h;
    }
    const RGBA* Screen::getPixels() const {
        mtx.lock();
        updated = false;
        mtx.unlock();
        return pixels;
    }
    void Screen::set(RGBA* pixels, int width, int height) {
        mtx.lock();
        updated = true;
        this->width = width;
        this->height = height;
        if (this->pixels!=nullptr)
            delete[] this->pixels;
        this->pixels = new RGBA[width*height];
        for (int i=0; i<width*height; i++) {
            this->pixels[i] = clamp(pixels[i]);
        }
        mtx.unlock();
    }
} // namespace NRenderer
