#pragma once
#ifndef __NR_SCENE_IMPORTER_FACTORY_HPP__
#define __NR_SCENE_IMPORTER_FACTORY_HPP__

#include <string>
#include <unordered_map>

#include "Importer.hpp"
#include "ScnImporter.hpp"
#include "ObjImporter.hpp"

namespace NRenderer
{
    using namespace std;
    class SceneImporterFactory
    {
    private:
        unordered_map<string, SharedImporter> importerMap;
    public:
        static SceneImporterFactory& instance() {
            static SceneImporterFactory f;
            return f;
        }
        SceneImporterFactory() {
            importerMap["scn"] = make_shared<ScnImporter>();
            importerMap["obj"] = make_shared<ObjImporter>();
        }
        SharedImporter importer(const string& ext) {
            auto it = importerMap.find(ext);
            if (it!=importerMap.end()) {
                return it->second;
            }
            return nullptr;
        }
    };
}

#endif