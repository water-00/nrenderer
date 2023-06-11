#pragma once
#ifndef __NR_INSTANCE_HPP__
#define __NR_INSTANCE_HPP__

#include <memory>
#include "common/macros.hpp"

namespace NRenderer
{
    using namespace std;
    struct DLL_EXPORT Instance: public enable_shared_from_this<Instance>
    {
        virtual ~Instance() = default;
    };
    using SharedInstance = shared_ptr<Instance>;
}

#endif