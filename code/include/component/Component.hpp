#pragma once
#ifndef __NR_COMPONENT_HPP__
#define __NR_COMPONENT_HPP__

#include "Instance.hpp"
#include <string>
#include "ComponentFactory.hpp"
namespace NRenderer
{
    using namespace std;
    class DLL_EXPORT Component: public Instance
    {
    private:
    protected:
    public:
        Component() = default;
        ~Component() = default;
    };
} // namespace NRenderer

#include "server/Server.hpp"

#define REGISTER_COMPONENT(__TYPE__, __NAME__, __DESCRIPTION__ , __CLASS__)                                                     \
        struct ComponentRegister {                                                                                              \
            ComponentRegister() {                                                                                               \
                getServer().componentFactory.registerComponent(#__TYPE__, #__NAME__, __DESCRIPTION__, std::make_shared<__CLASS__>);    \
            }                                                                                                                   \
            ~ComponentRegister() {                                                                                              \
                getServer().componentFactory.unregisterComponent(#__TYPE__, #__NAME__);                                                \
            }                                                                                                                   \
        };                                                                                                                      \
        static ComponentRegister __cm_reg__{};                                                                                  \

#define REGISTER_COMPONENT_NO_DESCRIPTION(__TYPE__, __NAME__, __CLASS__)   REGISTER_COMPONENT(__TYPE__, __NAME__, "", __CLASS__)

#endif