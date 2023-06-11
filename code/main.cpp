#include <iostream>

#include "ui/UI.hpp"

using namespace std;

#ifdef _WIN32
    #ifndef _DEBUG
        // #pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup")
    #endif
#endif

int main() {
    NRenderer::UI ui{1600, 900, "服务于本科教学的三维渲染系统"};
    try {
        ui.init();
    }
    catch (const exception& e) {
        cerr<<e.what()<<endl;
    }

    ui.run();
    
    return 0;
}