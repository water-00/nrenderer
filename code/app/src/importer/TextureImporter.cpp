#include "importer/TextureImporter.hpp"
#include "utilities/ImageLoader.hpp"
#include "utilities/GlImage.hpp"
#include "asset/TextureItem.hpp"
namespace NRenderer
{
    bool TextureImporter::import(Asset& asset, const string& path) {
        ImageLoader imgLoader;
        auto img = imgLoader.load(path);
        SharedTexture spTexture{new Texture()};
        spTexture->width = img->width;
        spTexture->height = img->height;
        spTexture->rgba = (RGBA*)img->data;
        auto id = GlImage::loadImage(spTexture->rgba, {spTexture->width, spTexture->height});
        TextureItem ti;
        ti.glId = id;
        ti.name = path;
        ti.texture = spTexture;
        asset.textureItems.push_back(move(ti));
        return true;
    }
}