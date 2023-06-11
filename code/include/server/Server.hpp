#pragma once
#ifndef __NR_SERVER_HPP__
#define __NR_SERVERHPP__

#include "Screen.hpp"
#include "Logger.hpp"
#include "component/ComponentFactory.hpp"

namespace NRenderer
{
    struct DLL_EXPORT Server
    {
        Logger logger = {};
        Screen screen = {};
        ComponentFactory componentFactory = {};
        Server() = default;
    };
} // namespace NRenderer

extern "C" {
    DLL_EXPORT
    NRenderer::Server& getServer();
}

#endif