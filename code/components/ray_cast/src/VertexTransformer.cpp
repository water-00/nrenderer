#include "VertexTransformer.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace RayCast
{
    void VertexTransformer::exec(SharedScene spScene) {
        auto& scene = *spScene;
        for (auto& node : scene.nodes) {
            Mat4x4 t{1};
            auto& model = spScene->models[node.model];
            t = glm::translate(t, model.translation);
            if (node.type == Node::Type::TRIANGLE) {
                for (int i=0; i<3; i++) {
                    auto& v = scene.triangleBuffer[node.entity].v[i];
                    v = t*Vec4{v, 1};
                }
            }
            else if (node.type == Node::Type::SPHERE) {
                auto& v = scene.sphereBuffer[node.entity].position;
                v = t*Vec4{v, 1};
            }
            else if (node.type == Node::Type::PLANE) {
                auto& v = scene.planeBuffer[node.entity].position;
                v = t*Vec4{v, 1};
            }
        }
    }
}