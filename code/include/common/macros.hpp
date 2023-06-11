#pragma once
#ifndef __NR_MACROS_HPP__
#define __NR_MACROS_HPP__

#ifdef _WIN32
    #define DLL_IMPORT __declspec(dllimport)
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_IMPORT
    #define DLL_EXPORT
#endif

#include <memory>

#define SHARE(__T__) using Shared##__T__ = std::shared_ptr<__T__>

#endif