#pragma once
#ifndef __NR_UI_HPP__
#define __NR_UI_HPP__

#include "glad/glad.h"
#include "glfw3.h"
#include "glfw3native.h"

#include "UIContext.hpp"
#include "manager/Manager.hpp"

#include <string>
#include <vector>

#include "views/View.hpp"

namespace NRenderer
{
    using namespace std;
    class UI
    {
    private:
        unsigned int height;
        unsigned int width;
        string title;
        GLFWwindow* window;

        Manager manager;
        UIContext uiContext;

        vector<View*> views;
    public:
        UI();
        UI(unsigned int width, unsigned int height, const string& title);
        UI(const UI&) = delete;
        ~UI();
        void init();
        void run();
    };
} // namespace NRenderer


#endif