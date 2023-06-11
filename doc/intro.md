代码说明
===

# 文件目录

```bash

---- code
  |- app            # ui系统, 资源管理, 资源导入等, 场景构建等
  |- components     # 插件
  |- dependences    # 外部依赖, glfw, glm, imgui, stb_image, glad等
  |- include        # 公共头文件
     |- component       # 插件系统头文件
     |- geometry        # 一些数学函数和包装glm
     |- scene           # 场景
     |- server          # 插件和ui系统交换数据的支持
  |- server         # 插件系统和插件与ui数据交换系统的实现
  |- test           # 单元测试
```

# 场景结构

```Cpp
struct Scene
{
    Camera camera;                                      // 摄像机

    RenderOption renderOption;                          // 渲染设置

    Ambient ambient;                                    // 环境光

    // buffers
    vector<Material> materials;                         // 场景中的所有材质
    vector<Texture> textures;                           // 场景中的所有纹理

    vector<Model> models;                               // 场景中的所有模型
    vector<Node> nodes;                                 // 场景中的所有节点
    // node所指向的实体
    vector<Sphere> sphereBuffer;                        // 球体 
    vector<Triangle> triangleBuffer;                    // 三角形
    vector<Plane> planeBuffer;                          // 平面
    vector<Mesh> meshBuffer;                            // 网格

    vector<Light> lights;                               // 场景中的所有灯光
    // light buffer
    vector<PointLight> pointLightBuffer;                // 点光源
    vector<AreaLight> areaLightBuffer;                  // 面光源(重要)
    vector<DirectionalLight> directionalLightBuffer;    // 方向光
    vector<SpotLight> spotLightBuffer;                  // 聚光灯
};

struct Model {
    vector<Index> nodes;                                // 包含节点的索引
    Vec3 translation = {0, 0, 0};                       // 在世界坐标的位置
    Vec3 scale = {1, 1, 1};                             // 缩放
};

struct Node
{
    enum class Type
    {
        SPHERE = 0x0,
        TRIANGLE = 0X1,
        PLANE = 0X2,
        MESH = 0X3
    };
    Type type = Type::SPHERE;                           // 节点类型
    Index entity;                                       // 实体的索引
    Index model;                                        // 所属的模型
};


struct Sphere : public Entity
{
    Vec3 direction = {0, 0, 1};                         // 球的方向(可以当作极轴的方向)
    Vec3 position = {0, 0, 0};                          // 位置(先对于模型, 局部坐标)
    float radius = { 0 };                               // 半径
};
    
struct Triangle : public Entity
{
    union {
        struct {
            Vec3 v1;
            Vec3 v2;
            Vec3 v3;
        };
        Vec3 v[3];                                      // 三个点的局部坐标
    };
    Vec3 normal;                                        // 法向量
    Triangle()
        : v1            ()
        , v2            ()
        , v3            ()
        , normal         (0, 0, 1)
    {}
};

struct Plane : public Entity
{
    Vec3 normal = {0, 0, 1};                            // 法向量
    Vec3 position = {};                                 // 位置(局部坐标)
    Vec3 u = {};                                        // 第一条边
    Vec3 v = {};                                        // 第二条边
};

struct Mesh : public Entity
{
    vector<Vec3> normals;                               // 法向量, 通过索引访问
    vector<Vec3> positions;                             // 位置, 通过索引访问
    vector<Vec2> uvs;                                   // uv向量, 通过索引访问
    vector<Index> normalIndices;                        // 法向量索引
    vector<Index> positionIndices;                      // 位置索引
    vector<Index> uvIndices;                            // uv索引

    bool hasNormal() const {
        return normals.size() != 0;
    }

    bool hasUv() const {
        return uvs.size() != 0;
    }
};


```

# 材质系统

```Cpp
struct Material
{
    Material() = default;
    
    unsigned int type = 0;

    vector<Property> properties;

    ...

};

```
**说明:**
+ `type` 决定是什么类型的材质, 自己定义, 一般情况下
  + `0` > Lambertian材质
  + `1` > 使用Phong光照模型的材质
  + `2` > Dielectric, 绝缘体, 例如玻璃等
  + `3` > Conductor, 导体, 例如金属
  + `4` > Plastic, 塑料
  + `>=5` > 自定义, 可以添加任何`property`
+ `properties` 材质属性

**如何使用(编写插件时):**
+ 创建一个接口类`Shader`
  ```cpp
    class Shader
    {
    public:
        virtual Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const = 0;
    };
  ```
+ 根据`type`, 分别继承`shader`, 并且实现接口, 例如
  ```cpp
    class Lambertian : public Shader
    {
    private:
        Vec3 albedo;
    public:
        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const;
    };

    Lambertian::Lambertian(Material& material)
        : Shader                (material, textures)
    {
        // 获取材质的Property
        auto diffuseColor = material.getProperty<Property::Wrapper::RGBType>("diffuseColor");
        // 如果材质存在此Property
        if (diffuseColor) albedo = (*diffuseColor).value;
        // 如果不存在的默认值
        else albedo = {1, 1, 1};
    }
  ```
+ 对每个材质根据其类型创建对应的`Shader`
  ```Cpp
    // SharedShader时shared_ptr<Shader>
    class ShaderCreator
    {
    public:
        ShaderCreator() = default;
        SharedShader create(Material& material, vector<Texture>& t) {
            SharedShader shader{nullptr};
            switch (material.type)
            {
            case 0:
                shader = make_shared<Lambertian>(material, t);
                break;
            case 1:
                ...
                break;
            case 2:
            case 3:
            ...
            case n:
            default:
                shader = make_shared<Lambertian>(material, t);
                break;
            }
            return shader;
        }
    };

    ...

    // 通过ShaderCreator, 创建Shader
    vector<SharedShader> shaderPrograms;

    ...

    // 使用
    auto scattered = shaderPrograms[mtlHandle.index()]->shade(r, hitObject->hitPoint, hitObject->normal);
  ```


# 支持的文件

场景/模型:
+ `.scn`
+ `.obj` - 仅支持三角化的网格

纹理:
+ `.jpg`
+ `.png`