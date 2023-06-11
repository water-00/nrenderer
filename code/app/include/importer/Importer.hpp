#pragma once
#ifndef __NR_IMPORTER_HPP__
#define __NR_IMPORTER_HPP__

#include <string>
#include "asset/Asset.hpp"

namespace NRenderer
{
    using namespace std;
    class Importer
    {
    protected:
        string lastErrorInfo;
    public:
        virtual bool import(Asset& asset, const string& path) = 0;
        inline
        string getErrorInfo() const {
            return lastErrorInfo;
        }
        Importer()
            : lastErrorInfo         ()
        {}
        virtual ~Importer() = default;
    };
    SHARE(Importer);
}

#endif