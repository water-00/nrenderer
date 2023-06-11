#pragma once
#ifndef __NR_SCN_IMPORTER_HPP__
#define __NR_SCN_IMPORTER_HPP__

#include "Importer.hpp"
#include <map>

namespace NRenderer
{
    using namespace std;
    class ScnImporter: public Importer
    {
    private:
        bool parseMtl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);
        bool parseMdl(Asset& asset, ifstream& file, map<string, size_t>& mtlMap);
        bool parseLgt(Asset& asset, ifstream& file);
    public:
        virtual bool import(Asset& asset, const string& path) override;
    };
}

#endif