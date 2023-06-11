#include "Metropolis.hpp"
#include "server/Server.hpp"
#include "VertexTransformer.hpp"
#include "intersections/intersections.hpp"
#include "glm/gtc/matrix_transform.hpp"

const auto taskNums = 8;
std::mutex mtx;

namespace Metropolis {
    RGB MetropolisRenderer::gamma(const RGB& rgb) {
        return glm::sqrt(rgb);
    }
    
    RGBA MetropolisRenderer::gamma(const RGBA& rgba) {
        return glm::sqrt(rgba);
    }

    RGBA MetropolisRenderer::gamma(const RGBA& rgba, unsigned long samples) {
        return Vec4{ pow(1 - exp(-rgba[0] * samples), 1 / 2.2), pow(1 - exp(-rgba[1] * samples), 1 / 2.2) ,
            pow(1 - exp(-rgba[2] * samples), 1 / 2.2), 0};
    }


    void MetropolisRenderer::renderTask(RGBA* pixels, int width, int height, int off, int step) {
        // 初始化Markov链
        TMarkovChain current, proposal;
        InitRandomNumbersByChain(current); // 将Metropolis类中的随机数数组赋值给current中的随机数数组
        current.C = CombinePaths(GenerateEyePath(MaxVertex), GenerateLightPath(MaxVertex)); // 用双向路径追踪生成新路径，每条未被遮挡的新路径都会在某一pixel(px, py)上做出contribution(c)，把所有有效的[px, py, c]赋值给current中记录PathContribution的数组C

        // 对路径进行mutation操作
        for (int i = 0; i < mutations; i++) {
            samps++; // 总采样次数
            double isLargeStepDone; // 取1或0，根据论文中的公式在有无进行large step mutation的情况下分别给contribution乘以不同的系数
            if (rnd() <= LargeStepProb) // 进行large step mutation的概率，LargeStepProb取0.3
            {
                proposal = large_step(current); // large_step()做的事就是重置current的所有随机数得到proposal，所以相当于生成一条全新的光路
                isLargeStepDone = 1.0;
            }
            else
            {
                proposal = mutate(current); // mutate()做的事就是根据论文中的公式对current中的随机数做扰动，得到proposal
                isLargeStepDone = 0.0;
            }
            InitRandomNumbersByChain(proposal); // 将Metropolis类中的随机数数组赋值给proposal中的随机数数组
            proposal.C = CombinePaths(GenerateEyePath(MaxVertex), GenerateLightPath(MaxVertex)); // 记录用两条子路径生成的所有有效的[px, py, c]

             // 计算接受新路径的概率
            double a;
            if (current.C.sc > 0.0)
                a = MAX(MIN(1.0, proposal.C.sc / current.C.sc), 0.0);

            // 根据论文中的公式将路径的contribution添加到照片上
            if (proposal.C.sc > 0.0)
                AccumulatePathContribution(pixels, proposal.C, (a + isLargeStepDone) / (proposal.C.sc / b + LargeStepProb));
            if (current.C.sc > 0.0)
                AccumulatePathContribution(pixels, current.C, (1.0 - a) / (current.C.sc / b + LargeStepProb));

            // 根据acceptance更新当前路径
            if (rnd() <= a)
                current = proposal;
        }

    }

    auto MetropolisRenderer::render() -> RenderResult {

        // shaders
        shaderPrograms.clear();
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            shaderPrograms.push_back(shaderCreator.create(m, scene.textures));
        }

        RGBA* pixels = new RGBA[width * height]{};

        // 局部坐标转换成世界坐标
        VertexTransformer vertexTransformer{};
        vertexTransformer.exec(spScene);


        // 用双向路径追踪估计照片最后的平均亮度b，b用于最后AccumulatePathContribution中的计算
        for (int i = 0; i < N_Init; i++) {
            InitRandomNumbers();
            Path eyeSubPath = GenerateEyePath(MaxVertex); // MaxVertex: 一条路径上允许的最大顶点数
            Path lightSubPath = GenerateLightPath(MaxVertex);
            double sc = CombinePaths(eyeSubPath, lightSubPath).sc; // 用双向路径追踪算法对两条子路径分别取前缀点、取后缀点生成最多MaxVertex^2条新路径，对这些新路径分别计算scalar contribution，取最大值加给b
            b += sc;
        }
        b /= double(N_Init);

        cout << "b = " << b << endl;

        thread t[taskNums];
        for (int i = 0; i < taskNums; i++) {
            t[i] = thread(&MetropolisRenderer::renderTask,
                this, pixels, width, height, i, taskNums);
        }
        for (int i = 0; i < taskNums; i++) {
            t[i].join();
        }




        getServer().logger.log("Done...");

        // 设置透明度以及gamma校正
        cout << "samps = " << samps << endl;
        double s = double(width * height) / double(samps);

        for (int ix = 0; ix < spScene->renderOption.width; ix++) {
            for (int iy = 0; iy < spScene->renderOption.height; iy++) {
                // 框架中的校正
                // pixels[ix + iy * width] = gamma(pixels[ix + iy * width] * (float)s);

                // MLT中的校正
                double gamma = 2.2;
                pixels[ix + iy * width][0] = pow(1 - exp(-pixels[ix + iy * width][0] * s), 1 / gamma);
                pixels[ix + iy * width][1] = pow(1 - exp(-pixels[ix + iy * width][1] * s), 1 / gamma);
                pixels[ix + iy * width][2] = pow(1 - exp(-pixels[ix + iy * width][2] * s), 1 / gamma);
                pixels[ix + iy * width][3] = 1.0;
            }
        }
            

        return { pixels, width, height };
    }

    void MetropolisRenderer::release(const RenderResult& r) {
        auto [p, w, h] = r;
        delete[] p;
    }

    HitRecord MetropolisRenderer::closestHitObject(const Ray& r) {
        int id = 0;

        HitRecord closestHit = nullopt;
        float closest = FLOAT_INF;
        for (auto& p : scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, 0.000001, closest, id);
            id++;
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        for (auto& s : scene.sphereBuffer) {
            auto hitRecord = Intersection::xSphere(r, s, 0.000001, closest, id);
            id++;
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        for (auto& t : scene.triangleBuffer) {
            auto hitRecord = Intersection::xTriangle(r, t, 0.000001, closest, id);
            id++;
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }

        return closestHit;
    }

    tuple<double, Vec3d, HitRecord> MetropolisRenderer::closestHitLight(const Ray& r) {
        Vec3d v = {};
        HitRecord closest = getHitRecord(FLOAT_INF, {}, {}, {}, {});
        for (auto& a : scene.areaLightBuffer) {
            auto hitRecord = Intersection::xAreaLight(r, a, 0.000001, closest->t);
            if (hitRecord && closest->t > hitRecord->t) {
                closest = hitRecord;
                v = a.radiance;
            }
        }
        return { closest->t, v, closest};
    }
    void MetropolisRenderer::getBox() {
        BVHTree* tree = new BVHTree(this->spScene);
        BVHNode* temp = new BVHNode();
        vector<Node> objects = spScene->nodes;
        temp->buildBounds(spScene, objects);
        box = temp->bounds;
    }

    void MetropolisRenderer::trace(Path &path, const Ray& r, int currDepth) {
        if (currDepth == depth) return;

        auto hitObject = closestHitObject(r);
        auto [t, emitted, hitLight] = closestHitLight(r);

        if (hitObject && hitObject->t < t) {
            // 把相交点信息添加进path
            Vec3d p = hitObject->hitPoint, n = glm::normalize(hitObject->normal);
            Vec3d dir = { r.direction.x, r.direction.y, r.direction.z };
            n = glm::dot(n, dir) < 0 ? n : n * -1.0; // 让n与r方向相反
            path.x[path.n] = Vert(p, n, hitObject->id);
            path.n++;
            const double rnd0 = prnds[(currDepth - 1) * NumRNGsPerEvent + 0 + PathRndsOffset];
            const double rnd1 = prnds[(currDepth - 1) * NumRNGsPerEvent + 1 + PathRndsOffset];

            // 对从交点发出的散射光线进行渲染
            Ray nr;
            nr.origin = p;
            nr.direction = VecCosine(n, 1.0, rnd0, rnd1); // 漫反射材质的反射光方向
            trace(path, nr, currDepth + 1);
        }
        else if (t != FLOAT_INF) {
            path.x[path.n] = Vert(hitLight->hitPoint, hitLight->normal, light_id);
            path.n++;
        }
    }

    // 返回沿下半球面一定角度范围内分布的单位向量
    Vec3d MetropolisRenderer::VecRandom(const double rnd1, const double rnd2)
    {
        const double temp1 = 2.0 * PI * rnd1; // [0, 2pi]
        const double temp2 = -0.6 + rnd2 * -0.2;

        const double s = sin(temp1);
        const double c = cos(temp1);
        const double t = sqrt(1.0 - temp2 * temp2);

        return Vec3d(s * t, temp2, c * t);
    }

    // 返回余弦分布的向量
    Vec3d MetropolisRenderer::VecCosine(const Vec3d n, const double g, const double rnd1, const double rnd2)
    {
        // g趋向于无穷，则函数返回结果就是n；g趋向于-1，函数返回（应该？）在球面上均匀分布的向量
        const double temp1 = 2.0 * PI * rnd1, temp2 = pow(rnd2, 1.0 / (g + 1.0));
        const double s = sin(temp1), c = cos(temp1), t = sqrt(1.0 - temp2 * temp2);
        Vec3d base = Vec3d(s * t, temp2, c * t);
        return onb(base, n);
    }
}