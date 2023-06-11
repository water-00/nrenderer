#include "server/Server.hpp"
#include "scene/Scene.hpp"
#include "component/RenderComponent.hpp"
#include "Camera.hpp"

#include "AccPathTracer.hpp"

using namespace std;
using namespace NRenderer;

namespace AccPathTracer
{
    class Adapter : public RenderComponent
    {
        void render(SharedScene spScene) {
            AccPathTracerRenderer renderer{spScene};
            auto renderResult = renderer.render();
            auto [ pixels, width, height ]  = renderResult;
            getServer().screen.set(pixels, width, height);
            renderer.release(renderResult);
        }
    };
}

const static string description = 
    "A Acc Path Tracer. "
    "Only some simple primitives and materials(Lambertian) are supported."
    "\nPlease use scene file : cornel_area_light.scn";

REGISTER_RENDERER(AccPathTracer, description, AccPathTracer::Adapter);