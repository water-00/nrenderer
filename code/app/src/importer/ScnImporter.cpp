#include "importer/ScnImporter.hpp"

#include <fstream>
#include <sstream>

#include <map>

namespace NRenderer
{
    bool ScnImporter::parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        SharedMaterial spMaterial{nullptr};

        bool successFlag = true;
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token=="") continue;
            else if (token[0] == '#') continue;
            else if (token == "Material") {
                // get Name
                string name;
                ss>>name;
                MaterialItem item;
                spMaterial = SharedMaterial{new Material()};
                item.material = spMaterial;
                item.name = name;
                auto index = asset.materialItems.size();
                asset.materialItems.push_back(item);
                if (mtlMap.find(name) == mtlMap.end()) {
                    mtlMap[name] = index;
                }
                else {
                    lastErrorInfo = "Duplicated Material Key:" + name;
                    successFlag = false;
                    break;
                }
                // get Type
                unsigned int type = 0;
                if (!ss.eof())
                    ss>>type;
                spMaterial->type = type;
            }
            else if (token == "Prop") {
                using PT = Property::Type;
                using PW = Property::Wrapper;
                string key, type;
                ss>>key>>type;
                if (type == "Int") {
                    int v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::IntType{v});
                }
                else if (type == "Float") {
                    float v;
                    ss>>v;
                    spMaterial->registerProperty(key, PW::FloatType{v});
                }
                else if (type == "Vec3") {
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::Vec3Type{Vec3{v1, v2, v3}});
                }
                else if (type == "Vec4") {
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::Vec4Type{Vec4{v1, v2, v3, v4}});
                }
                else if (type == "RGB") {
                    float v1, v2, v3;
                    ss>>v1>>v2>>v3;
                    spMaterial->registerProperty(key, PW::RGBType{Vec3{v1, v2, v3}});
                }
                else if (type == "RGBA") {
                    float v1, v2, v3, v4;
                    ss>>v1>>v2>>v3>>v4;
                    spMaterial->registerProperty(key, PW::RGBAType{Vec4{v1, v2, v3, v4}});
                }
            }
            else if (token == "End") {
                break;
            }
            else {
                successFlag = false;
                break;
            }
        }

        return successFlag;
    }

    bool ScnImporter::parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap) {
        string currline;
        
        stringstream ss{};
        
        ModelItem modelItem;

        bool successFlag = true;

        int currNodeType = 0;
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token=="") continue;
            else if (token[0] == '#') continue;
            else if (token == "Model") {
                modelItem = ModelItem{};
                ss>>modelItem.name;
                modelItem.model = make_shared<Model>();
                asset.modelItems.push_back(modelItem);
            }
            else if (token == "Translation") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->translation = {f1, f2, f3};
            }
            else if (token == "Scale") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                (asset.modelItems.end() - 1)->model->scale = {f1, f2, f3};
            }
            else if (token == "Sphere") {
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::SPHERE;
                currNodeType = 0;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.spheres.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.spheres.push_back(SharedSphere{new Sphere()});
                asset.spheres[asset.spheres.size() - 1]->material = mtl->second;
            }
            else if (token == "Triangle") {
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::TRIANGLE;
                currNodeType = 1;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                ni.node->entity = asset.triangles.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                asset.nodeItems.push_back(ni);
                asset.triangles.push_back(SharedTriangle{new Triangle()});
                asset.triangles[asset.triangles.size() - 1]->material = mtl->second;
            }
            else if (token == "Plane") {
                NodeItem ni{};
                ss>>ni.name;
                ni.node = SharedNode{new Node{}};
                ni.node->type = Node::Type::PLANE;
                currNodeType = 2;
                string mtlName;
                ss>>mtlName;
                auto mtl = mtlMap.find(mtlName);
                if (mtl == mtlMap.end()) {
                    lastErrorInfo = string("Invalid material name.");
                    successFlag = false;
                    break;
                }
                asset.modelItems[asset.modelItems.size() - 1].model->nodes.push_back(asset.nodeItems.size());
                ni.node->entity = asset.planes.size();
                ni.node->model = asset.modelItems.size() - 1;
                asset.nodeItems.push_back(ni);
                asset.planes.push_back(SharedPlane{new Plane()});
                asset.planes[asset.planes.size() - 1]->material = mtl->second;
            }
            else if (token == "R") {
                // radius of sphere
                float f;
                ss>>f;
                auto it = asset.spheres.end()-1;
                (*it)->radius = f;

            }
            else if (token == "N") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 n = {f1, f2, f3};
                if (currNodeType == 0) {
                    auto it = asset.spheres.end() - 1;
                    (*it)->direction = n;
                }
                else if (currNodeType == 1) {
                    auto it = asset.triangles.end() - 1;
                    (*it)->normal = n;
                } 
                else if (currNodeType == 2) {
                    auto it = asset.planes.end() - 1;
                    (*it)->normal = n;
                }
            }
            else if (token == "V1") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[0] = v;
            }
            else if (token == "V2") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[1] = v;
            }
            else if (token == "V3") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.triangles.end() - 1;
                (*it)->v[2] = v;
            }
            else if (token == "P") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 p = {f1, f2, f3};
                if (currNodeType == 0) {
                    auto it = asset.spheres.end() - 1;
                    (*it)->position = p;
                }
                else if (currNodeType == 2) {
                    auto it = asset.planes.end() - 1;
                    (*it)->position = p;
                }
            }
            else if (token == "U") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->u = u;
            }
            else if (token == "V") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v = {f1, f2, f3};
                auto it = asset.planes.end() - 1;
                (*it)->v = v;
            }
            else if (token == "End") {
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    bool ScnImporter::parseLgt(Asset& asset, ifstream& file) {
        bool successFlag = true;

        string currline;
        stringstream ss{};

        int currLightType{0};
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (token == "") continue;

            else if (token[0] == '#') continue;

            else if (token == "Point") {
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 0;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::POINT;
                li.light->entity = asset.pointLights.size();
                asset.lightItems.push_back(li);
                asset.pointLights.push_back(make_shared<PointLight>());
            }
            else if (token == "Spot") {
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 3;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::SPOT;
                li.light->entity = asset.spotLights.size();
                asset.lightItems.push_back(li);
                asset.spotLights.push_back(make_shared<SpotLight>());
            }
            else if (token == "Directional") {
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 2;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::DIRECTIONAL;
                li.light->entity = asset.directionalLights.size();
                asset.lightItems.push_back(li);
                asset.directionalLights.push_back(make_shared<DirectionalLight>());
            }
            else if (token == "Area") {
                LightItem li{};
                string name;
                ss>>name;
                currLightType = 1;
                li.name = name;
                li.light = SharedLight{new Light()};
                li.light->type = Light::Type::AREA;
                li.light->entity = asset.areaLights.size();
                asset.lightItems.push_back(li);
                asset.areaLights.push_back(make_shared<AreaLight>());
            }
            // Value irradiance, radiance or intensity
            else if (token == "IRV") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                if (currLightType == 0) {
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.intensity = v;
                }
                else if (currLightType == 1) {
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.radiance = v;
                }
                else if (currLightType == 2) {
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.irradiance = v;
                }
                else if (currLightType == 3) {
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.intensity = v;
                }
            }
            else if (token == "P") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 pos{f1, f2, f3};
                if (currLightType == 0) {
                    auto& p = *(*(asset.pointLights.end() - 1));
                    p.position = pos;
                }
                else if (currLightType == 1) {
                    auto& a = *(*(asset.areaLights.end() - 1));
                    a.position = pos;
                }
                else if (currLightType == 3) {
                    auto& s = *(*(asset.spotLights.end() - 1));
                    s.position = pos;
                }
            }
            else if (token == "D") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 dir{f1, f2, f3};
                if (currLightType == 2) {
                    auto& d = *(*(asset.directionalLights.end() - 1));
                    d.direction = dir;
                }
                else if (currLightType == 3) {
                    auto& s = *(*(asset.directionalLights.end() - 1));
                    s.direction = dir;
                }
            }
            else if (token == "HotSpot") {
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.hotSpot = r;
            }
            else if (token == "Fallout") {
                float r;
                ss>>r;
                auto& s = *(*(asset.spotLights.end() - 1));
                s.fallout = r;
            }
            else if (token == "U") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 u{ f1, f2, f3 };
                auto& a = *(*(asset.areaLights.end() - 1));
                a.u = u;
            }
            else if (token == "V") {
                float f1, f2, f3;
                ss>>f1>>f2>>f3;
                Vec3 v{f1, f2, f3};
                auto& a = *(*(asset.areaLights.end() - 1));
                a.v = v;
            }
            else if (token == "End") {
                break;
            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
                break;
            }
        }
        return successFlag;
    }

    bool ScnImporter::import(Asset& asset, const string& path) {
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

        string currline;
        stringstream ss{};

        map<string, size_t> mtlMap;
        
        while(getline(file, currline)) {
            ss.str("");
            ss.clear();
            string token;
            ss<<currline;
            ss>>token;
            if (successFlag == false) {
                break;
            }
            if (token == "") continue;

            else if (token[0] == '#') continue;

            else if (token == "Begin") {
                ss>>token;
                if (token == "Material") {
                    successFlag = parseMtl(asset, file, mtlMap);
                }
                else if (token == "Model") {
                    successFlag = parseMdl(asset, file, mtlMap);
                }
                else if (token == "Light") {
                    successFlag = parseLgt(asset, file);
                }
                else {
                    successFlag = false;
                }

            }
            else {
                successFlag = false;
                lastErrorInfo = "Syntax Error!";
            }

            ss.clear();
            ss.str("");
        }
        if (successFlag) {
            for (auto i = beginNode; i < asset.nodeItems.size(); i++) {
                asset.genPreviewGlBuffersPerNode(asset.nodeItems[i]);
            }

            for (auto i = beginLight; i < asset.lightItems.size(); i++) {
                asset.genPreviewGlBuffersPerLight(asset.lightItems[i]);
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