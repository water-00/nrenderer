#include "component/RenderComponent.hpp"
#include "server/Server.hpp"


namespace RenderExample
{
    using namespace std;
    using namespace NRenderer;

    // 继承RenderComponent, 复写render接口
    class Adapter : public RenderComponent
    {
        void render(SharedScene spScene) {
            getServer().logger.log("Simply Output some color");
            _sleep(1000);

            int height = spScene->renderOption.height;
            int width = spScene->renderOption.width;
            RGBA* pixels = new RGBA[height*width]{};
            for (int i=0; i<height; i++) {
                for (int j=0; j<width; j++) {
                    pixels[i*width+j] = {float(i)/float(height), float(j)/float(width), 1.f, 1.f};
                }
            }
            // 输出颜色
            getServer().screen.set(pixels, width, height);
            delete[] pixels;

            // logger
            getServer().logger.log("common...");
            getServer().logger.success("success...");
            getServer().logger.warning("warning...");
            getServer().logger.error("error...");
        }
    };
}

// REGISTER_RENDERER(Name, Description, Class)
REGISTER_RENDERER(Example, "Simply output some color. (This is description)", RenderExample::Adapter);