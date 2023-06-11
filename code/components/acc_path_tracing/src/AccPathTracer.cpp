#include "server/Server.hpp"
#include "AccPathTracer.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Onb.hpp"

const auto taskNums = 16;
AccPathTracer::Timer timers[taskNums]{};

namespace AccPathTracer
{

    RGB AccPathTracerRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }

    void AccPathTracerRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        for (int i = off; i < height; i += step) {
            for (int j = 0; j < width; j++) {
                Vec3 color{ 0, 0, 0 };
                for (int k = 0; k < samples; k++) {
                    auto r = defaultSamplerInstance<UniformInSquare>().sample2d(); // 随机生成采样点
                    float rx = r.x;
                    float ry = r.y;
                    // 归一化
                    float x = (float(j) + rx) / float(width);
                    float y = (float(i) + ry) / float(height);
                    auto ray = camera.shoot(x, y); // 每次朝每一个pixel (j, i)内部的一点(j + rx, i + ry)发射光线，发射samples次
                    color += trace(ray, 0, off);
                }
                color /= samples;
                color = gamma(color);
                pixels[(height - i - 1) * width + j] = { color, 1 };
            }
        }
        

    }

    auto AccPathTracerRenderer::render() -> RenderResult {
        for (int i = 0; i < taskNums; i++) {
            timers[i].init();
        }
        // shaders
        shaderPrograms.clear();
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(m, scene.textures,spScene->renderOption));
        }

        RGBA* pixels = new RGBA[width * height]{};

        // 局部坐标转换成世界坐标
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);
        
        //在这里生成Bounds
        getBox();
        tree = new BVHTree(spScene);
        tree->root = tree->build(box);
        //-------------
        thread t[taskNums];
        for (int i = 0; i < taskNums; i++) {
            t[i] = thread(&AccPathTracerRenderer::renderTask,
                this, pixels, width, height, i, taskNums);
        }
        for (int i = 0; i < taskNums; i++) {
            t[i].join();
        }
        getServer().logger.log("Done...");

        float total_ms = 0.0;
        for (int i = 0; i < taskNums; i++) {
            total_ms += timers[i].getTime();
        }
        cout << "threadNum = " << taskNums << ", closestHitObject time per thread with BVH: " << total_ms / taskNums / 1000.0 << "s." << endl;

        return { pixels, width, height };
    }

    void AccPathTracerRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    HitRecord AccPathTracerRenderer::closestHitObject(const Ray& r) {
        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;

        // BVH
        auto hitRecord = tree->Intersect(r, closest);
        if (hitRecord && hitRecord->t < closest ) {
            closest = hitRecord->t;
            closestHit = hitRecord;
        }

        return closestHit;
    }

    tuple<float, Vec3> AccPathTracerRenderer::closestHitLight(const Ray& r) {
        Vec3 v = {};
        HitRecord closest = getHitRecord(FLOAT_INF, {}, {}, {});
        for (auto& a : scene.areaLightBuffer) {
            auto hitRecord = Intersection::xAreaLight(r, a, 0.000001, closest->t);
            if (hitRecord && closest->t > hitRecord->t) {
                closest = hitRecord;
                v = a.radiance;
            }
        }
        return { closest->t, v };
    }
    void AccPathTracerRenderer::getBox() {
        BVHTree* tree = new BVHTree(this->spScene);
        BVHNode* temp = new BVHNode();
        vector<Node> objects = spScene->nodes;
        temp->buildBounds(spScene, objects);
        box = temp->bounds;
    }

    RGB AccPathTracerRenderer::trace(const Ray& r, int currDepth, int thread_id) {
        if (currDepth == depth) return scene.ambient.constant;

        timers[thread_id].start();
        auto hitObject = closestHitObject(r);
        timers[thread_id].stop();

        auto [t, emitted] = closestHitLight(r);
        //如果光源与交点之间有物体间隔，则计算的是间接光照
        if (hitObject && hitObject->t < t) {
            auto mtlHandle = hitObject->material;
            auto scattered = shaderPrograms[mtlHandle.index()]->shade(r, hitObject->hitPoint, hitObject->normal);
            if (spScene->materials[mtlHandle.index()].type == 0) {
                auto scatteredRay = scattered.ray;
                auto attenuation = scattered.attenuation;
                auto emitted = scattered.emitted;
                //由于漫反射发出的光线可言任意方向，因此这里用的是random随机选取的反射光线
                auto next = trace(scatteredRay, currDepth + 1, thread_id);
                float n_dot_in = glm::dot(hitObject->normal, scatteredRay.direction);//cos(N法向量,L光源)
                float pdf = scattered.pdf;
                /**
                    * emitted      - Le(p, w_0)
                    * next         - Li(p, w_i)
                    * n_dot_in     - cos<n, w_i>
                    * atteunation  - BRDF
                    * pdf          - p(w)
                    **/
                    //emitted:直接光照发出的强度
                    //后部分是漫反射
                return emitted + attenuation * next * n_dot_in / pdf;
            }
            else if (spScene->materials[mtlHandle.index()].type == 2) {//玻璃
                auto reflex = scattered.data.reflex;
                auto reflex_rate = scattered.data.reflex_rate;
                auto refraction = scattered.data.refraction;
                auto refraction_rate = scattered.data.refraction_rate;
                auto reflex_emit = trace(reflex, currDepth + 1, thread_id);
                auto refraction_emit = reflex_rate==Vec3(0.f)?RGB(0.f):trace(refraction, currDepth + 1, thread_id);
                return reflex_emit*reflex_rate + refraction_emit * refraction_rate;
            }
            else if (spScene->materials[mtlHandle.index()].type == 1) {//光滑导体
                auto L = scattered.ray;
                auto attenuation = scattered.attenuation;
                auto next = trace(L, currDepth + 1, thread_id);
                return attenuation * next;
            }
            else if (spScene->materials[mtlHandle.index()].type == 3) {
                auto L = scattered.ray;
                auto attenuation = scattered.attenuation;
                auto next = trace(L, currDepth + 1, thread_id);
                return attenuation * next;
            }
            
        }
        else if (t != FLOAT_INF) {//直接光照
            return emitted;
        }
        else {
            return Vec3{ 0 };
        }
    }
}