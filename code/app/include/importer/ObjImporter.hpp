#pragma once
#ifndef __OBJ_IMPORTER_HPP__
#define __OBJ_IMPORTER_HPP__

#include "Importer.hpp"
#include <map>

#include <unordered_map>

namespace NRenderer
{
    using namespace std;
    class ObjImporter: public Importer
    {
    private:
        bool parseMtl(Asset& asset, const string& path, ifstream& file, unordered_map<string, size_t>& mtlMap);
    public:
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif