#pragma once
#ifndef __NR_ASSET_MANAGER_HPP__
#define __NR_ASSET_MANAGER_HPP__
#include "asset/Asset.hpp"
#include "importer/TextureImporter.hpp"
#include "utilities/FileFetcher.hpp"
#include "importer/SceneImporterFactory.hpp"
#include "utilities/File.hpp"
#include "server/Server.hpp"
namespace NRenderer
{
    struct AssetManager
    {
        Asset asset;
        void importScene() {
            FileFetcher ff;
            auto optPath = ff.fetch("All\0*.scn;*.obj\0");
            if (optPath) {
                auto importer = SceneImporterFactory::instance().importer(File::getFileExtension(*optPath));
                bool success = importer->import(asset, *optPath);
                if (!success) {
                    getServer().logger.error(importer->getErrorInfo());
                }
                else {
                    getServer().logger.success("成功导入:" + *optPath);
                }
            }
        }
        void StaticimportScene() {
            //FileFetcher ff;
            //auto optPath = ff.fetch("All\0*.scn;*.obj\0");
            auto optPath = string("../../../../../resource/path_tracing_cornel.scn");
            auto importer = SceneImporterFactory::instance().importer(File::getFileExtension(optPath));
            bool success = importer->import(asset, optPath);
            if (!success) {
                getServer().logger.error(importer->getErrorInfo());
            }
            else {
                getServer().logger.success("成功导入:" + optPath);
            }
        }
        void importTexture() {
            TextureImporter tImp{};
            FileFetcher ff;
            auto optPath = ff.fetch("image\0*.png;*.jpg\0");
            if (optPath) {
                tImp.import(asset, *optPath);
            }
        }
        void clearAll() {
            asset.clearModel();
            asset.clearLight();
            asset.clearMaterial();
            asset.clearTexture();
        }
        void clearModel() {
            asset.clearModel();
        }
        void clearLight() {
            asset.clearLight();
        }
        // void glGenVaoForAllNodes();
    };
}


#endif
