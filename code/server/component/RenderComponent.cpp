#include "component/RenderComponent.hpp"

namespace NRenderer
{
    void RenderComponent::exec(function<void()> onStart, function<void()> onFinish, SharedScene spScene) {
        onStart();
        render(spScene);
        onFinish();
    }
} // namespace Renderer