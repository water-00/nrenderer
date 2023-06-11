#pragma once
#ifndef __METROPOLIS_HPP__
#define __METROPOLIS_HPP__

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"
#include "Timer.hpp"
#include "shaders/ShaderCreator.hpp"
#include "Bounds3.hpp"
#include "BVH.hpp"
#include "Onb.hpp"
#include "TKahanAdder.hpp"
#include "PathContribution.hpp"
#include "TMarkovChian.hpp"
#include <algorithm>
#include <tuple>


namespace Metropolis
{

    using namespace NRenderer;
    using namespace std;

 
    class MetropolisRenderer
    {
    private:
        unsigned long mutations = 2000000;
        int PathRndsOffset = 0;
        double prnds[NumStates];
        Vec3d emitted{ 47.8384 * 2.0, 38.5664 * 2.0, 31.0808 * 2.0 };
        double b = 0.0;


        SharedScene spScene;
        Scene& scene;

        unsigned int width;
        unsigned int height;
        unsigned int depth;
        unsigned int samples;
        unsigned int acc_type;

        unsigned long samps = 0; // 采样数

        using SCam = Metropolis::Camera;
        SCam camera;

        vector<SharedShader> shaderPrograms;
    public:
        MetropolisRenderer(SharedScene spScene)
            : spScene(spScene)
            , scene(*spScene)
            , camera(spScene->camera)
        {
            width = scene.renderOption.width;
            height = scene.renderOption.height;
            depth = scene.renderOption.depth;
            samples = scene.renderOption.samplesPerPixel;
            acc_type = scene.renderOption.acc_type;

            // for BVH
            getBox();
            tree = new BVHTree(spScene);
            tree->root = tree->build(box);
        }
        ~MetropolisRenderer() = default;

        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;
        RenderResult render();
        void release(const RenderResult& r);

        // BVH
        vector<Bounds3> box;
        BVHTree* tree;
        void getBox();

        // 用两个随机数 in [0, 1)^2 生成一个终点在半球面上的向量
        Vec3d VecRandom(const double rnd1, const double rnd2);

        // 构造一个按余弦分布的随机方向，更倾向于与法线接近的方向
        Vec3d VecCosine(const Vec3d n, const double g, const double rnd1, const double rnd2);

        // 将double映射到[0, 255]，并进行了gamma校正
        int toInt(double x) { return int(pow(1 - exp(-x), 1 / 2.2) * 255 + .5); }



        // xorshift PRNG，生成[0, 1) double
        inline double rnd() {
            static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
            unsigned int t = x ^ (x << 11);
            x = y;
            y = z;
            z = w;
            return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8))) * (1.0 / 4294967296.0);
        }

        // 根据s1, s2扰动value
        inline double perturb(const double value, const double s1, const double s2)
        {
            double Result;
            double r = rnd();
            if (r < 0.5)
            {
                r = r * 2.0;
                Result = value + s2 * exp(-log(s2 / s1) * r);
                if (Result > 1.0)
                    Result -= 1.0;
            }
            else
            {
                r = (r - 0.5) * 2.0;
                Result = value - s2 * exp(-log(s2 / s1) * r);
                if (Result < 0.0)
                    Result += 1.0;
            }
            return Result;
        }

        // large_step就是生成一条与之前完全无关的光路，那就是重置随机数
        TMarkovChain large_step(TMarkovChain MC)
        {
            TMarkovChain Result;
            Result.C = MC.C;
            for (int i = 0; i < NumStates; i++)
                Result.u[i] = rnd();
            return Result;
        }

        TMarkovChain mutate(TMarkovChain MC)
        {
            TMarkovChain Result;
            Result.C = MC.C;

            // pixel location
            Result.u[0] = perturb(MC.u[0], 2.0 / double(width + height), 0.1);
            Result.u[1] = perturb(MC.u[1], 2.0 / double(width + height), 0.1);

            // the rest
            for (int i = 2; i < NumStates; i++)
                Result.u[i] = perturb(MC.u[i], 1.0 / 1024.0, 1.0 / 64.0);
            return Result;
        }

        void InitRandomNumbersByChain(const TMarkovChain MC)
        {
            for (int i = 0; i < NumStates; i++)
                prnds[i] = MC.u[i];
        }
        void InitRandomNumbers()
        {
            for (int i = 0; i < NumStates; i++)
                prnds[i] = rnd();
        }

        // local sampling PDFs and standard terms
        inline double GeometryTerm(const Vert e0, const Vert e1)
        {
            const Vec3d dv = e1.p - e0.p;
            const double d2 = glm::dot(dv, dv);
            return fabs(glm::dot(e0.n, dv) * glm::dot(e0.n, dv)) / (d2 * d2);
        }
        inline double DirectionToArea(const Vert current, const Vert next)
        {
            const Vec3d dv = next.p - current.p;
            const double d2 = glm::dot(dv, dv); // 两点距离
            return fabs(glm::dot(next.n, dv)) / (d2 * sqrt(d2));
        }

        inline double LambertianBRDF(const Vec3d wi, const Vec3d n, const Vec3d wo)
        {
	        return 1.0 / PI;
        }
        inline double LambertianPDF(const Vec3d wi, const Vec3d n, const Vec3d wo)
        {
	        return fabs(glm::dot(wo, n)) / PI;
        }

        // 返回当前向量在与给定法线向量构建的正交规范化基下的坐标表示
        inline Vec3d onb(const Vec3d& base, const Vec3d& n) const
        {
            // 用n计算出一组正交基向量u, w, v, 返回base在这组正交基下的坐标
            Vec3d u, w, v = n;
            if (n.z < -0.9999999)
            {
                u = Vec3d(0.0, -1.0, 0.0);
                w = Vec3d(-1.0, 0.0, 0.0);
            }
            else
            {
                const double a = 1.0 / (1.0 + n.z);
                const double b = -n.x * n.y * a;
                u = Vec3d(1.0 - n.x * n.x * a, b, -n.x);
                w = Vec3d(b, 1.0 - n.y * n.y * a, -n.y);
            }
            return Vec3d(glm::dot(base, Vec3d(u.x, v.x, w.x)), glm::dot(base, Vec3d(u.y, v.y, w.y)), glm::dot(base, Vec3d(u.z, v.z, w.z)));
        }
  
        // 获取Path Xb上第i个Vert的color
        Vec3d getColor(const Path& Xb, int i) {
            // 获取color的思路：用id找到它在nodes中的实体类型(plane/triangle/sphere)，然后可以通过这个实体的material.index()获取，然后通过if语句结合.scn文件中的数值得到颜色
            int color_index = -1;
            Vec3d color{ 0 };
            int id = Xb.x[i].id;
            if (id == -3) {
                return emitted;

            }

            auto node = scene.nodes[id];
            if (node.type == Node::Type::SPHERE) {
                color_index = scene.sphereBuffer[node.entity].material.index();
            }
            else if (node.type == Node::Type::TRIANGLE) {
                color_index = scene.triangleBuffer[node.entity].material.index();
            }
            else if (node.type == Node::Type::PLANE) {
                color_index = scene.planeBuffer[node.entity].material.index();
            }
            else if (node.type == Node::Type::MESH) {
                color_index = scene.meshBuffer[node.entity].material.index();
            }

            if (color_index == 0)
                color = { 0.725, 0.71, 0.68 };
            else if (color_index == 1)
                color = { 0.63, 0.065, 0.05 };
            else if (color_index == 2)
                color = { 0.14, 0.45, 0.091 };

            return color;
        }

        // measurement contribution function
        Vec3d PathThroughput(const Path Xb)
        {
            Vec3d f = Vec3d(1.0, 1.0, 1.0);
            for (int i = 0; i < Xb.n; i++)
            {
                // 第一个点，camera
                if (i == 0)
                {
                    double W = 1.0 / double(width * height); // 每个pixel的weight
                    Vec3d d0 = Xb.x[1].p - Xb.x[0].p; // camera指向第一个反射点
                    const double dist2 = glm::dot(d0, d0); // camera到第一个反射点的距离平方
                    d0 = d0 * (1.0f / sqrt(dist2)); // d0成了一个单位向量
                    const double c = glm::dot(d0, -camera.w); // d0的z分量
                    double dist = height / (2.0 * camera.halfHeight); // camera到中心平面的垂线距离
                    // double dist = camera.focusDis;
                    const double ds2 = (dist / c) * (dist / c); // camera到中心平面沿d0方向的距离平方
                    W = W / (c / ds2);
                    f = f * (W * fabs(glm::dot(d0, Xb.x[1].n) / dist2));
                }
                // 最后一个点，light source
                else if (i == (Xb.n - 1))
                {
                    if (Xb.x[i].id == light_id) // 这样写应该可以吧，判断一个点的reflect类型是不是光源的方法就是去看它的id
                    {
                        const Vec3d d0 = glm::normalize((Xb.x[i - 1].p - Xb.x[i].p));
                        const double L = LambertianBRDF(d0, Xb.x[i].n, d0);
                        // 光源的color，直接从.scn文件里找
                        f = f * (emitted * L);
                    }
                    else
                    {
                        f = f * 0.0;
                    }
                }
                // 中间的点，物体表面
                else
                {
                    const Vec3d d0 = glm::normalize((Xb.x[i].p - Xb.x[i - 1].p));
                    const Vec3d d1 = glm::normalize((Xb.x[i + 1].p - Xb.x[i].p));
                    double BRDF = LambertianBRDF(d0, Xb.x[i].n, d1);
                    Vec3d color = getColor(Xb, i);
                    double g = GeometryTerm(Xb.x[i], Xb.x[i + 1]);
                    if (!isnormal(g))
                        continue;

                    f = f * (color * BRDF * g);
                }
                if (Max(f) <= 0)
                    return Vec3d{ 0, 0, 0 };

                if (!std::isnormal(f.x))
                    cout << "i = " << i << ", f = " << f << endl;
            }
            return f;
        }

        // check if the path can be connected or not (visibility term)
        bool isConnectable(const Path Xeye, const Path Xlight, double& px, double& py)
        {
            // TODO
            Vec3d Direction;

            // 取出两条路线的最后一个点
            const Vert& Xeye_e = Xeye.x[Xeye.n - 1];
            const Vert& Xlight_e = Xlight.x[Xlight.n - 1];

            bool Result; 
            if ((Xeye.n == 0) && (Xlight.n >= 2))
            {
                // no direct hit to the film (pinhole)
                Result = false;
            }
            else if ((Xeye.n >= 2) && (Xlight.n == 0))
            {
                // Xeye path有点而Xlight path没点，就判断Xeye path最后一个点是不是光源
                Result = (Xeye_e.id == light_id);
                Direction = glm::normalize((Xeye.x[1].p - Xeye.x[0].p));
            }
            else if ((Xeye.n == 1) && (Xlight.n >= 1))
            {
                // Xeye只有camera这一个点，就连接camera和Xlight的最后一个点做求交，如果交点就是Xlight的最后一个点说明没有遮挡
                Ray r(Xeye.x[0].p, glm::normalize((Xlight_e.p - Xeye.x[0].p)));
                auto hitObject = closestHitObject(r);
                Result = (hitObject && (hitObject->id == Xlight_e.id));
                Direction = r.direction;
            }
            else
            {
                // shadow ray connection
                Ray r(Xeye_e.p, glm::normalize((Xlight_e.p - Xeye_e.p))); // 生成一条从eye subpath最后一点指向light subpath最后一点的路径，检查是否会与物体相交
                auto hitObject = closestHitObject(r);
                Result = (hitObject && (hitObject->id == Xlight_e.id)); // 如果连接光线与场景的相交点是light subpath最后一点，那么说明没被遮挡
                Direction = glm::normalize((Xeye.x[1].p - Xeye.x[0].p)); // 新光线的出发方向
            }

            // get the pixel location 得到新光路的Direction在image plane上的位置(px, py)，检查(px, py)是否在image内
            // TODO 这一部分的计算还是好复杂...但我觉得我现在的理解是对的
            double dist = height / (2.0 * camera.halfHeight);
            // double dist = camera.focusDis;
            Vec3d ScreenCenter = camera.position + (-camera.w * dist); // 屏幕中心，(0, 0, 683.8694)
            Vec3d ScreenPosition = camera.position + (Direction * (dist / glm::dot(Direction, -camera.w))) - ScreenCenter; // 新光路与image plane(z = 0)的交点
            px = glm::dot(camera.u, ScreenPosition) + (width * 0.5); // 通过引用修改px py的位置，返回给上层
            py = glm::dot(-camera.v, ScreenPosition) + (height * 0.5);

            //cout << "ScreenPosition = " << ScreenPosition << ", px = " << px << ", py = " << py << endl;
            return Result && ((px >= 0) && (px < width) && (py >= 0) && (py < height));
        }

        // path probability density
        // - take the sum of all possible probability densities if the numbers of subpath vertices are not specified
        double PathProbablityDensity(const Path SampledPath, const int PathLength, const int SpecifiedNumEyeVertices = -1, const int SpecifiedNumLightVertices = -1)
        {
            TKahanAdder SumPDFs(0.0);
            bool Specified = (SpecifiedNumEyeVertices != -1) && (SpecifiedNumLightVertices != -1); // 指定了两个子路径上的顶点数量

            double dist = height / (2.0 * camera.halfHeight);
            // double dist = camera.focusDis;
            // number of eye subpath vertices
            for (int NumEyeVertices = 0; NumEyeVertices <= PathLength + 1; NumEyeVertices++)
            {
                // extended BPT
                double p = 1.0;

                // number of light subpath vertices
                int NumLightVertices = (PathLength + 1) - NumEyeVertices;

                // we have pinhole camera
                if (NumEyeVertices == 0)
                    continue;

                // add all?
                if (Specified && ((NumEyeVertices != SpecifiedNumEyeVertices) || (NumLightVertices != SpecifiedNumLightVertices)))
                    continue;

                // sampling from the eye
                for (int i = -1; i <= NumEyeVertices - 2; i++)
                {
                    if (i == -1)
                    {
                        // PDF of sampling the camera position (the same delta function with the scaling 1.0 for all the PDFs - they cancel out)
                        p = p * 1.0;
                    }
                    else if (i == 0)
                    {
                        p = p * 1.0 / double(width * height);
                        Vec3d Direction0 = glm::normalize(SampledPath.x[1].p - SampledPath.x[0].p);
                        double CosTheta = glm::dot(Direction0, -camera.w);
                        double DistanceToScreen2 = dist / CosTheta;
                        DistanceToScreen2 = DistanceToScreen2 * DistanceToScreen2;
                        p = p / (CosTheta / DistanceToScreen2);

                        p = p * DirectionToArea(SampledPath.x[0], SampledPath.x[1]);
                    }
                    else
                    {
                        // PDF of sampling ith vertex
                        if ((SampledPath.x[i - 1].p == SampledPath.x[i].p) || (SampledPath.x[i].p == SampledPath.x[i + 1].p))
                            continue;
                        Vec3d Direction0 = glm::normalize(SampledPath.x[i - 1].p - SampledPath.x[i].p);
                        Vec3d Direction1 = glm::normalize(SampledPath.x[i + 1].p - SampledPath.x[i].p);
                        p = p * LambertianPDF(Direction0, SampledPath.x[i].n, Direction1);
                        p = p * DirectionToArea(SampledPath.x[i], SampledPath.x[i + 1]);
                    }
                }

                if (p != 0)
                {
                    // sampling from the light source
                    for (int i = -1; i <= NumLightVertices - 2; i++)
                    {
                        if (i == -1)
                        {
                            // PDF of sampling the light position (assume area-based sampling)
                            double lightArea = abs(scene.areaLightBuffer[0].u.x * scene.areaLightBuffer[0].v.z);
                            p = p * (1.0 / lightArea);
                        }
                        else if (i == 0)
                        {
                            Vec3d Direction0 = glm::normalize(SampledPath.x[PathLength - 1].p - SampledPath.x[PathLength].p);
                            p = p * LambertianPDF(SampledPath.x[PathLength].n, SampledPath.x[PathLength].n, Direction0);
                            p = p * DirectionToArea(SampledPath.x[PathLength], SampledPath.x[PathLength - 1]);
                        }
                        else
                        {
                            // PDF of sampling (PathLength - i)th vertex
                            if ((SampledPath.x[PathLength - (i - 1)].p == SampledPath.x[PathLength - i].p) || (SampledPath.x[PathLength - i].p == SampledPath.x[PathLength - (i + 1)].p))
                                continue;
                            Vec3d Direction0 = glm::normalize(SampledPath.x[PathLength - (i - 1)].p - SampledPath.x[PathLength - i].p);
                            Vec3d Direction1 = glm::normalize(SampledPath.x[PathLength - (i + 1)].p - SampledPath.x[PathLength - i].p);
                            p = p * LambertianPDF(Direction0, SampledPath.x[PathLength - i].n, Direction1);
                            p = p * DirectionToArea(SampledPath.x[PathLength - i], SampledPath.x[PathLength - (i + 1)]);
                        }
                    }
                }

                if (Specified && (NumEyeVertices == SpecifiedNumEyeVertices) && (NumLightVertices == SpecifiedNumLightVertices))
                    return p;

                // sum the probability density (use Kahan summation algorithm to reduce numerical issues)
                SumPDFs.add(p);
            }
            return SumPDFs.sum;
        }

        Ray SampleLightSources(const double rnd1, const double rnd2)
        {
            // base1, base2是面光源的对角线上的两个点
            auto& areaLight = scene.areaLightBuffer[0];
            Vec3d base1 = areaLight.position + areaLight.u;
            Vec3d base2 = areaLight.position + areaLight.v;
            double x_len = std::fabs(base1.x - base2.x), z_len = std::fabs(base1.z - base2.z); // 考虑到面光源与xz平面平行，就这么写了
            Vec3d p = Vec3d{ min(base1.x, base2.x) + rnd1 * x_len, base1.y, min(base1.z, base2.z) + rnd2 * z_len };

            Vec3d n = { 0, -1, 0 }; // 光源的法向量，就直接这么写在这里了

            // 不同的光源方向分布方式
            Vec3d d = VecCosine(n, 999.0, prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            // Vec3d d = VecCosine(n, 2.0, prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            // Vec3d d = VecCosine(n, -0.5, prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            // Vec3d d = VecRandom(prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            // Vec3d d = { 0.0, 0.0, -1.0 };
            PathRndsOffset += NumRNGsPerEvent;

            return Ray{p, d};
        }
        Path GenerateLightPath(const int MaxLightEvents)
        {
            // 初始化路径采样结果Result
            Path Result;
            Result.n = 0;
            if (MaxLightEvents == 0)
                return Result;
            for (int i = 0; i < MaxVertex; i++)
                Result.x[i].id = -1;
            PathRndsOffset = NumStatesSubpath;

            // 从光源采样一条光线
            Ray r = SampleLightSources(prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            PathRndsOffset += NumRNGsPerEvent;

            // Result第一个点放光源的信息
            auto& areaLight = scene.areaLightBuffer[0];
            Vec3d n = glm::normalize(glm::cross(areaLight.u, areaLight.v));
            Result.x[0] = Vert(r.origin, n, light_id);
            Result.n++;

            // 对这条光线去采样生成完整路径
            trace(Result, r, 1);
            return Result;
        }

        Ray SampleCamera(const double rnd1, const double rnd2)
        {
            // 根据u, v, w的比例进行采样，可以确保采样方向能打到目标范围
            const Vec3d su = camera.u * -(0.5 - rnd1) * (double)width;
            const Vec3d sv = camera.v * (0.5 - rnd2) * (double)height;
            double dist = height / (2.0 * camera.halfHeight); // camera到中心平面的垂线距离
            // double dist = camera.focusDis;
            const Vec3d sw = -camera.w * dist;
            return Ray(camera.position, glm::normalize(su + sv + sw));
        }
        Path GenerateEyePath(const int MaxEyeEvents)
        {
            // 初始化路径采样结果Result
            Path Result;
            Result.n = 0;
            if (MaxEyeEvents == 0)
                return Result;
            // 初始化Result中的顶点
            for (int i = 0; i < MaxVertex; i++) {
                Result.x[i].id = -1; // -1 表示该顶点还没有东西，另外light_id = -3, camera_id = -2
            }
            PathRndsOffset = 0;

            // 从camera采样一束光线
            // Ray r = camera.shoot(prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]); // 这里应该可以用这个函数替代SampleCamera()吧--得到的b更大
            Ray r = SampleCamera(prnds[PathRndsOffset + 0], prnds[PathRndsOffset + 1]);
            PathRndsOffset += NumRNGsPerEvent;

            // Result第一个点放camera信息
            Result.x[0] = Vert(r.origin, camera.w, camera_id);
            Result.n++;

            // 对这条光线去采样生成完整路径
            trace(Result, r, 1);
            return Result;
        }

        // balance heuristic
        double MISWeight(const Path SampledPath, const int NumEyeVertices, const int NumLightVertices, const int PathLength)
        {
            const double p_i = PathProbablityDensity(SampledPath, PathLength, NumEyeVertices, NumLightVertices);
            const double p_all = PathProbablityDensity(SampledPath, PathLength);
            if ((p_i == 0.0) || (p_all == 0.0))
            {
                return 0.0;
            }
            else
            {
                return MAX(MIN(p_i / p_all, 1.0), 0.0);
            }
        }

        // BPT connections 双向路径追踪中的连接
        // - limit the connection to a specific technique if s and t are provided
        PathContribution CombinePaths(const Path EyePath, const Path LightPath, const int SpecifiedNumEyeVertices = -1, const int SpecifiedNumLightVertices = -1)
        {
            // TODO 此函数做的事情是：在EyePath中找前缀在LightPath中找后缀，组合成新路径，记录该路径的contribution(px, py, c)
            PathContribution Result;
            Result.n = 0;
            Result.sc = 0.0;

            for (int PathLength = MinPathLength; PathLength <= MaxPathLength; PathLength++) {
                for (int NumEyeVertices = 0; NumEyeVertices <= PathLength + 1; NumEyeVertices++)
                {
                    // 每次循环产生一条新路径

                    const int NumLightVertices = (PathLength + 1) - NumEyeVertices;

                    // 依次选EyePath的前NumEyeVertices个点和LightPath前NumLightVertices个点，且PathLength逐渐变长
                    if (NumEyeVertices == 0)
                        continue; // no direct hit to the film (pinhole) 成像平面？
                    if (NumEyeVertices > EyePath.n)
                        continue;
                    if (NumLightVertices > LightPath.n)
                        continue;

                    // extract subpaths
                    Path Eyesubpath = EyePath;
                    Path Lightsubpath = LightPath;
                    Eyesubpath.n = NumEyeVertices;
                    Lightsubpath.n = NumLightVertices;

                    // check the path visibility
                    double px = -1.0, py = -1.0;
                    // 在isConnectable里面如果是可连接的话，px, py就是新路径贡献的位置(即camera与image plane的交点)
                    if (!isConnectable(Eyesubpath, Lightsubpath, px, py))
                        continue;

                    // 既然通过了isConnectable检查，那SampledPath就是新光路的那些点，小索引的是eyepath的点，大索引的是lightpath的点，并且第一个点是camera最后一个点是light source
                    Path SampledPath;
                    for (int i = 0; i < NumEyeVertices; i++)
                        SampledPath.x[i] = EyePath.x[i];
                    for (int i = 0; i < NumLightVertices; i++)
                        SampledPath.x[PathLength - i] = LightPath.x[i];
                    SampledPath.n = NumEyeVertices + NumLightVertices;

                    // evaluate the path
                    Vec3d f = PathThroughput(SampledPath);
                    double p = PathProbablityDensity(SampledPath, PathLength, NumEyeVertices, NumLightVertices);
                    double w = MISWeight(SampledPath, NumEyeVertices, NumLightVertices, PathLength);

                    if ((w <= 0) || (p <= 0))
                        continue;

                    Vec3d c = f * (w / p);
                    if (Max(c) <= 0)
                        continue;

                    // store the pixel contribution
                    Result.c[Result.n] = Contribution(px, py, c);
                    Result.n++;

                    // scalar contribution function
                    Result.sc = MAX(Max(c), Result.sc);
                }
            }

            return Result;
        }

        void AccumulatePathContribution(RGBA* pixels, const PathContribution pc, const double mScaling)
        {
            // TODO
            if (pc.sc == 0)
                return;
            for (int i = 0; i < pc.n; i++)
            {
                const int ix = int(pc.c[i].x), iy = int(pc.c[i].y); // 通过顶点找到其贡献的照片的像素位置
                Vec3d c = pc.c[i].c * mScaling;
                if ((ix < 0) || (ix >= width) || (iy < 0) || (iy >= height))
                    continue;
                pixels[ix + iy * width] += Vec4{ c, 0 }; // TODO 不确定应该怎么写
            }
        }

    private:
        void renderTask(RGBA* pixels, int width, int height, int off, int step);

        RGB gamma(const RGB& rgb);
        RGBA gamma(const RGBA& rgba, unsigned long samples);
        RGBA gamma(const RGBA& rgba);
        void trace(Path &path, const Ray& ray, int currDepth);
        HitRecord closestHitObject(const Ray& r);
        tuple<double, Vec3d, HitRecord> closestHitLight(const Ray& r);

    };



}

#endif