#pragma once
#ifndef __NR_TEXTURE_IMPORTER_HPP__
#define __NR_TEXTURE_IMPORTER_HPP__

#include "Importer.hpp"

namespace NRenderer
{
    class TextureImporter : public Importer
    {
    public:
        TextureImporter() = default;
        ~TextureImporter() = default;
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif