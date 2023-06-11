#pragma once
#ifndef __NR_COMPONENT_FACTORY_HPP__
#define __NR_COMPONENT_FACTORY_HPP__

#include "Instance.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace NRenderer
{
    struct DLL_EXPORT ComponentInfo {
        string id;
        string type;
        string name;
        string description;
    };
    struct DLL_EXPORT ComponentWrapper
    {
        ComponentInfo info;
        function<SharedInstance()> constructor;
    };
    class DLL_EXPORT ComponentFactory final
    {
    public:
        ComponentFactory() = default;
        virtual ~ComponentFactory() = default;
        bool registerComponent(const string& type, const string& name, const string& description, function<SharedInstance()> constructor);
        void unregisterComponent(const string& type, const string& name);
        template<typename T>
        shared_ptr<T> createComponent(const string& type, const string& name) {
            return static_pointer_cast<T>(createInstance(type, name));
        }
    
        vector<ComponentInfo> getComponentsInfo(const string& type = "");
    protected:
        SharedInstance createInstance(const string& type, const string& name);
    private:
        static string getId(const string& type, const string& id) {
            string prefix = "NR";
            return prefix + "." + type + "." + id;
        }
        unordered_map<string, unordered_map<string, ComponentWrapper> > constructors;
    };
} // namespace NRenderer

/*
extern "C" {
    DLL_EXPORT
    NRenderer::ComponentFactory& getComponentFactory();
}
*/

#endif