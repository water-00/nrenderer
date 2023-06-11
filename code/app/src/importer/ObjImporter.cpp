#include <fstream>
#include <sstream>

#include "utilities/File.hpp"
#include "utilities/ImageLoader.hpp"
#include "utilities/GlImage.hpp"

#include "importer/ObjImporter.hpp"

namespace NRenderer
{
    inline
    TextureItem loadTexture(const string& filePath, const string& fileName) {
        ImageLoader imageLoader{};
        auto pImage = imageLoader.load(filePath+fileName, 4);
        auto& image = *pImage;
        if (pImage == nullptr) 
            return {};
        auto ti = TextureItem{};
        ti.texture = SharedTexture{new Texture{}};
        ti.name = fileName;
        ti.texture->width = image.width;
        ti.texture->height = image.height;
        ti.texture->rgba = new RGBA[image.width*image.height];
        for (int i = 0; i < image.width * image.height; i++) {
            ti.texture->rgba[i].r = image.data[i*4];
            ti.texture->rgba[i].g = image.data[i*4 + 1];
            ti.texture->rgba[i].b = image.data[i*4 + 2];
            ti.texture->rgba[i].a = image.data[i*4 + 3];
        }
        ti.glId = GlImage::loadImage(ti.texture->rgba, {image.width, image.height});
        return ti;
    }
    bool ObjImporter::parseMtl(Asset& asset, const string& path, ifstream& file, unordered_map<string, size_t>& mtlMap) {
        MaterialItem* currMaterialItem = nullptr;

        using PW = Property::Wrapper;

        string currLine;
        string token;

        stringstream ss{};

        bool successFlag = true;

        while(getline(file, currLine)) {
            ss<<currLine;
            ss>>token;

            transform(token.begin(), token.end(), token.begin(), ::tolower);

            if(token == "newmtl") {
                ss>>token;
                mtlMap.insert({token, asset.materialItems.size() - 1});

                asset.materialItems.push_back({});
                currMaterialItem = &asset.materialItems[asset.materialItems.size() - 1];
                currMaterialItem->material = make_shared<Material>();
                currMaterialItem->material->type = 1;
                currMaterialItem->name = token;
            }
            else if (token == "ka") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                // 不支持Ka
            }
            else if (token == "kd") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                auto diffuseColor = Vec3{f1, f2, f3};
                currMaterialItem->material->registerProperty("diffuseColor", PW::RGBType{diffuseColor});
            }
            else if (token == "ks") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                auto specularColor = Vec3{f1, f2, f3};
                currMaterialItem->material->registerProperty("specularColor", PW::RGBType{specularColor});
            }
            else if (token == "ns") {
                float f;
                ss>>f;
                auto specularEx = f;
                currMaterialItem->material->registerProperty("specularEx", PW::FloatType{ f });
            }
            else if (token == "d") {
                float f;
                ss>>f;
                // currMaterial->transparency = 1 - f;
            }
            else if (token == "ke") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                // currMaterial->emissive = { f1, f2, f3 };
            }
            else if (token == "tr") {
                float f;
                ss>>f;
                // currMaterial->transparency = f;
            }
            // 仅支持RGB, 不支持CIEXYZ
            else if (token == "tf") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                // currMaterial->transmissionFilterColor = Vec3{f1, f2, f3};
            }
            else if (token == "ni") {
                float f;
                ss>>f;
                // currMaterial->refractionIndex = f;
            }
            else if (token == "map_ka" || token == "map_d" || token == "disp" || token == "decal") {
                // 不支持
            }
            else if (token == "map_kd") {
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "diffuseMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            else if (token == "map_ks") {
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "specularMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            else if (token == "map_bump" || token == "bump") {
                ss>>token;
                auto ti = loadTexture(path, token);
                if (ti.texture != nullptr) {
                    Handle t{ (unsigned int)asset.textureItems.size() };
                    Property p{ "bumpMap", PW::TextureIdType{t} };
                    currMaterialItem->material->registerProperty(p);
                    asset.textureItems.push_back(ti);
                }
            }
            ss.clear();
            ss.str("");
        }
        return successFlag;
    }

    bool ObjImporter::import(Asset& asset, const string& path) {
        ifstream file(path);
        if (!file.is_open()) {
            lastErrorInfo = "File does not exist!";
            return false;
        }
        size_t beginModel = asset.modelItems.size();
        size_t beginNode = asset.nodeItems.size();
        size_t beginMaterial = asset.materialItems.size();
        size_t beginTexture = asset.textureItems.size();

        size_t beginSph = asset.spheres.size();
        size_t beginTri = asset.triangles.size();
        size_t beginPln = asset.planes.size();
        size_t beginMsh = asset.meshes.size();

        size_t beginLight = asset.lightItems.size();
        size_t beginPnt = asset.pointLights.size();
        size_t beginArea = asset.areaLights.size();
        size_t beginDir = asset.directionalLights.size();
        size_t beginSpt = asset.spotLights.size();
        
        
        bool successFlag = true;

        auto modelName = File::getFileName(path);
        ModelItem modelItem;
        modelItem.name = modelName;
        modelItem.model = make_shared<Model>();

        string currLine;
        bool hasMtl = false;
        
        string token;
        stringstream ss{};

        unordered_map<string, size_t> mtlMap;
        
        vector<Vec3> positions;
        vector<Vec3> normals;
        vector<Vec2> uvs;

        using _mp = shared_ptr<unordered_map<Index, Index>>;
        _mp pMap{nullptr};
        _mp tMap{nullptr};
        _mp nMap{nullptr};

        NodeItem* currNodePtr = nullptr;

        Handle currUsedMtl{};

        while (getline(file, currLine)) {
            ss<<currLine;
            ss>>token;

            if (token == "mtllib") {
                string mtlFileName;
                ss>>mtlFileName;
                auto npos = path.find_last_of("\\/");
                string mtlPath;
                mtlPath = path.substr(0, npos + 1);
                auto mtlFilePath = mtlPath + mtlFileName;
                ifstream mtlFile(mtlFilePath);
                if (!mtlFile.is_open()) {
                    successFlag = false;
                    lastErrorInfo = "Cannot file .mtl file";
                    break;
                }
                successFlag = parseMtl(asset, mtlPath, mtlFile, mtlMap);
                if (successFlag) {
                    hasMtl = true;
                }
                else {
                    break;
                }
            }
            else if (token == "usemtl") {
                string mtlName;
                ss>>mtlName;
                auto mtlItr = mtlMap.find(mtlName);
                if (mtlItr != mtlMap.end()) {
                    currUsedMtl.setIndex(mtlItr->second);
                    asset.meshes[currNodePtr->node->entity]->material = currUsedMtl;
                }
                else {
                    successFlag = false;
                    lastErrorInfo = "Cannot find material: " + mtlName;
                    break;
                }
            }
            // v： 顶点
            else if (token == "v") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                positions.push_back({ f1, f2, f3 });
            }
            // vt：顶点纹理坐标
            else if (token == "vt") {
                float f1, f2;
                ss>>f1>>f2;
                uvs.push_back({ f1, f2 });
            }
            // vn：顶点法向量
            else if (token == "vn") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                normals.push_back({ f1, f2, f3 });
            }
            // o：对象
            else if (token == "o" || token == "g") {
                string name;
                if (ss.eof()) {
                    name = "undefined";
                }
                else {
                    ss>>name;
                }
                modelItem.model->nodes.push_back(asset.nodeItems.size());
                asset.nodeItems.push_back({});
                pMap = make_shared<unordered_map<Index, Index> >();
                tMap = make_shared<unordered_map<Index, Index> >();
                nMap = make_shared<unordered_map<Index, Index> >();
                currNodePtr = &(*(asset.nodeItems.end() - 1));
                currNodePtr->name = name;
                currNodePtr->node = make_shared<Node>();
                currNodePtr->node->type = Node::Type::MESH;
                currNodePtr->node->model = asset.modelItems.size();
                currNodePtr->node->entity = asset.meshes.size();
                asset.meshes.push_back(make_shared<Mesh>());

                // cout<<"new object"<<endl;
            }
            // f: 面, 必须是三角化的
            else if (token == "f") {
                if (currNodePtr == nullptr) {
                    modelItem.model->nodes.push_back(asset.nodeItems.size());
                    asset.nodeItems.push_back({});
                    pMap = make_shared<unordered_map<Index, Index> >();
                    tMap = make_shared<unordered_map<Index, Index> >();
                    nMap = make_shared<unordered_map<Index, Index> >();
                    currNodePtr = &(*(asset.nodeItems.end() - 1));
                    currNodePtr->name = "Undefined";
                    currNodePtr->node = make_shared<Node>();
                    currNodePtr->node->type = Node::Type::MESH;
                    currNodePtr->node->model = asset.modelItems.size();
                    currNodePtr->node->entity = asset.meshes.size();
                    asset.meshes.push_back(make_shared<Mesh>());
                }

                long v{0}, t{0}, n{0};
                char c = '\0';

                exception e(".obj file must be triangulated.");

                Index vpi[3];
                Index vti[3];
                Index vni[3];
                for (int i=0; i<3; i++) {
                    // example: f v/t/n v/t/n v/t/n

                    vpi[i] = -1;
                    vti[i] = -1;
                    vni[i] = -1;

                    if(ss.eof()) throw e;

                    string vertexToken;
                    ss>>vertexToken;
                    stringstream vertexStream{vertexToken};
                    auto begin = vertexToken.find_first_of('/');
                    auto end = vertexToken.find_last_of('/');
                    // f v v v
                    if (begin == vertexToken.npos) {
                        vertexStream>>v;
                        t = -1;
                        n = -1;
                    }
                    // f v/t v/t v/t
                    else if (begin == end) {
                        vertexStream>>v>>c>>t;
                        n = -1;
                    }
                    // f v//n v//n v//n
                    else if (begin + 1 == end) {
                        vertexStream>>v>>c>>c>>n;
                        t = -1;
                    }
                    else {
                        vertexStream>>v>>c>>t>>c>>n;
                    }

                    if (v != -1) {
                       // 找到
                        auto finded = pMap->find(v);
                        if (finded != pMap->end()) {
                            vpi[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->positions.size();
                            vpi[i] = idx;
                            pMap->insert({v, idx});
                            asset.meshes[currNodePtr->node->entity]->positions.push_back(positions[v - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->positionIndices.push_back(vpi[i]);
                    }
                    if (t != -1) {
                        auto finded = tMap->find(t);
                        if (finded != tMap->end()) {
                            vti[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->uvs.size();
                            vti[i] = idx;
                            tMap->insert({t, idx});
                            asset.meshes[currNodePtr->node->entity]->uvs.push_back(uvs[t - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->uvIndices.push_back(vti[i]);

                    }
                    if (n != -1) {
                        auto finded = nMap->find(n);
                        if (finded != nMap->end()) {
                            vni[i] = finded->second;
                        }
                        else {
                            auto idx = asset.meshes[currNodePtr->node->entity]->normals.size();
                            vni[i] = idx;
                            nMap->insert({n, idx});
                            asset.meshes[currNodePtr->node->entity]->normals.push_back(normals[n - 1]);
                        }
                        asset.meshes[currNodePtr->node->entity]->normalIndices.push_back(vni[i]);
                    }
                }
                if (!ss.eof()) {
                    successFlag = false;
                    lastErrorInfo = "Only Triangulated mesh is supported!";
                    break;
                }
            }
            ss.clear();
            ss.str("");
        }

        asset.modelItems.push_back(modelItem);

        if (successFlag) {
            for (auto i = beginNode; i < asset.nodeItems.size(); i++) {
                asset.genPreviewGlBuffersPerNode(asset.nodeItems[i]);
            }
        }

        if (!successFlag) {
            asset.modelItems        .erase(asset.modelItems         .begin() + beginModel,      asset.modelItems.end());
            asset.nodeItems         .erase(asset.nodeItems          .begin() + beginNode,       asset.nodeItems.end());
            asset.materialItems     .erase(asset.materialItems      .begin() + beginMaterial,   asset.materialItems.end());
            asset.textureItems      .erase(asset.textureItems       .begin() + beginTexture,    asset.textureItems.end());
            
            asset.spheres           .erase(asset.spheres            .begin() + beginSph,        asset.spheres.end());
            asset.triangles         .erase(asset.triangles          .begin() + beginTri,        asset.triangles.end());
            asset.planes            .erase(asset.planes             .begin() + beginPln,        asset.planes.end());
            asset.meshes            .erase(asset.meshes             .begin() + beginMsh,        asset.meshes.end());
            
            asset.lightItems        .erase(asset.lightItems         .begin() + beginLight,      asset.lightItems.end());
            asset.pointLights       .erase(asset.pointLights        .begin() + beginPnt,        asset.pointLights.end());
            asset.areaLights        .erase(asset.areaLights         .begin() + beginArea,       asset.areaLights.end());
            asset.directionalLights .erase(asset.directionalLights  .begin() + beginDir,        asset.directionalLights.end());
            asset.spotLights        .erase(asset.spotLights         .begin() + beginSpt,        asset.spotLights.end());
        }

        return successFlag;
    }
}