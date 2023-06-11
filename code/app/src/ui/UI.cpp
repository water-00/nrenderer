#include "ui/UI.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "geometry/vec.hpp"
#include "server/Server.hpp"

#include "ui/views/LogView.hpp"
#include "ui/views/AssetView.hpp"
#include "ui/views/SceneView.hpp"
#include "ui/views/ScreenView.hpp"
#include "ui/views/ComponentProgressView.hpp"

namespace NRenderer
{
    UI::UI()
        : width             (1600)
        , height            (900)
        , title             ("服务于本科教学的三维渲染系统")
        , window            (nullptr)
        , uiContext         ()
        , views             ()
    {}
    UI::UI(unsigned int width, unsigned int height, const string& title)
        : width             (width)
        , height            (height)
        , title             (title)
        , window            (nullptr)
        , uiContext         ()
        , views             ()
    {}
    UI::~UI() {
        for (auto pView : views) {
            delete pView;
        }
    }

    void UI::init() {
        if (glfwInit() == GLFW_FALSE)
            throw "GLFW fail to initialize.";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);
        glfwMakeContextCurrent(window);
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw "GLAD fail to initialize";

        // glfw
        if (!window) {
            glfwTerminate();
            throw "Fail to create window";
        }

    #pragma region _INIT_
        views.push_back(new LogView({0, 600}, {900, 300}, uiContext, manager));
        views.push_back(new AssetView({900, 0}, {700, 500}, uiContext, manager));
        views.push_back(new ScreenView({0, 0}, {600, 600}, uiContext, manager));
        views.push_back(new SceneView({600, 0}, {300, 600}, uiContext, manager));
        views.push_back(new ComponentProgressView({}, {240, 100}, uiContext, manager));
    #pragma endregion _INIT_
    }

    void UI::run() {
        const char* glslVersion = "#version 330";
        auto background = Vec4(0.3f, 0.4f, 0.5f, 1.0f);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();  
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplOpenGL3_Init(glslVersion);
        ImGui_ImplGlfw_InitForOpenGL(window, true);

        glfwSwapInterval(1);

        getServer().logger.success("欢迎使用(*^_^*), 程序还有很多不完善的地方, 遇到问题希望大家积极反映.");

        while(!glfwWindowShouldClose(window)) {
            // Poll events
            glClearColor(background.x, background.y, background.z, background.w);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

        #pragma region IMGUI_RENDER
            
            // ImGui::ShowDemoWindow();

            for (auto view : views) {
                view->display();
            }
         
        #pragma endregion IMGUI_RENDER

        #pragma region OPENGL_RENDER
        #pragma endregion OPENGL_RENDER

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);

            glfwSwapBuffers(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
} // namespace NRenderer
