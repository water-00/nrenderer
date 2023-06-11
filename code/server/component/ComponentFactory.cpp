#include "component/ComponentFactory.hpp"
#include "component/Component.hpp"

NRenderer::ComponentFactory& getComponentFactory() {
    static NRenderer::ComponentFactory cf{};
    return cf;
}

namespace NRenderer
{
    bool ComponentFactory::registerComponent(const string& type, const string& name, const string& desc, function<SharedInstance()> constructor) {
        auto& ncMap = constructors[type];
        if (ncMap.find(name) == ncMap.end()) {
            ComponentInfo ci{getId(type, name), type, name, desc};
            ComponentWrapper cw{ci, constructor};
            ncMap[name] = cw;
            return true;
        }
        else {
            throw "Target Component: " + getId(type, name) + " has already been registered. Check your registration info.";
            return false;
        }
    }

    void ComponentFactory::unregisterComponent(const string& type, const string& name) {
        auto& ncMap = constructors[type];
        ncMap.erase(name);
    }

    SharedInstance ComponentFactory::createInstance(const string& type, const string& name) {
        auto& ncMap = constructors[type];
        if (ncMap.find(name) != ncMap.end()) {
            return ncMap[name].constructor();
        }
        else {
            return nullptr;
        }
    }

    vector<ComponentInfo> ComponentFactory::getComponentsInfo(const string& type) {
        vector<ComponentInfo> infos;
        if (type == "") {
            for (auto& ncMap : constructors) {
                for (auto& it : ncMap.second) {
                    infos.push_back(it.second.info);
                }
            } 
        }
        else {
            auto ncMapIt = constructors.find(type);
            if (ncMapIt != constructors.end()) {
                for (auto& it : ncMapIt->second) {
                    infos.push_back(it.second.info);
                }
            }
        }
        return std::move(infos);
    }
} // namespace NRenderer
