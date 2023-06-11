#include "server/Server.hpp"
#include "component/RenderComponent.hpp"

#include "RayCastRenderer.hpp"

using namespace std;
using namespace NRenderer;

namespace RayCast
{
    class Adapter : public RenderComponent
    {
    public:
        void render(SharedScene spScene) {
            RayCastRenderer rayCast{spScene};
            auto result = rayCast.render();
            auto [ pixels, width, height ] = result;
            getServer().screen.set(pixels, width, height);
            rayCast.release(result);
        }
    };
}

const static string description = 
    "Ray Cast Renderer.\n"
    "Supported:\n"
    " - Lambertian and Phong\n"
    " - One Point Light\n"
    " - Triangle, Sphere, Plane\n"
    " - Simple Pinhole Camera\n\n"
    "Please use ray_cast.scn"
    ;

REGISTER_RENDERER(RayCast, description, RayCast::Adapter);
