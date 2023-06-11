#include "VertexTransformer.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace SimplePathTracer
{
    void VertexTransformer::exec(SharedScene spScene) {
        auto& scene = *spScene;
        for (auto& node : scene.nodes) {
            Mat4x4 t{ 1 };
            auto& model = spScene->models[node.model];
            t = glm::translate(t, model.translation);
            if (node.type == Node::Type::TRIANGLE) {
                for (int i = 0; i < 3; i++) {
                    auto& v = scene.triangleBuffer[node.entity].v[i];
                    v = t * Vec4{ v, 1 };
                }
            }
            else if (node.type == Node::Type::SPHERE) {
                auto& v = scene.sphereBuffer[node.entity].position;
                v = t * Vec4{ v, 1 };
            }
            else if (node.type == Node::Type::PLANE) {
                auto& v = scene.planeBuffer[node.entity].position;
                v = t * Vec4{ v, 1 };
            }
            else if (node.type == Node::Type::MESH) {
                // TODO 如果是.obj，把所有点平移+缩放到和其他点差不多的位置的大小

                // 这个glm库的矩阵构造方式真的好奇怪...和matlab反着来的
                // for 200/1k bunny
                //Mat4x4 t = {
                //    400, 0, 0, 0,
                //    0, 400, 0, 0,
                //    0, 0, 400, 0,
                //    40, -305, 920, 1
                //};

                // for 5k bunny
                Mat4x4 t = {
                    600, 0, 0, 0,
                    0, 600, 0, 0,
                    0, 0, 600, 0,
                    40, -305, 920, 1
                };

                Mesh& buffer = scene.meshBuffer[node.entity];
                for (int i = 0; i < buffer.positions.size(); i += 1) {
                    Vec4 v = t * Vec4{ buffer.positions[i] , 1 };
                    buffer.positions[i] = v;
                }
            }
        }
    }
}