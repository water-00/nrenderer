#include "manager/ComponentManager.hpp"
#include "io.h"

namespace NRenderer
{
    ComponentManager::ComponentManager()
        : state             (State::IDLING)
        , loadedDlls        ()
        , activeComponent   ()
        , lastStartTime     ()
        , lastEndTime       ()
        , t                 ()
    {}

    void ComponentManager::init(const string& path) {
        auto& f = getServer().componentFactory;
        _finddata_t findData;
        auto handle = _findfirst(path.c_str(), &findData);
        if (handle == -1) return;
        do {
            string name{findData.name};
            string filePath = (path.substr(0 ,path.size() - 6) + "\\" + name);
            
            auto h = ::LoadLibrary((LPCSTR)filePath.c_str());
            if (h != NULL) {
                loadedDlls.push_back(h);
            }
        } while (_findnext(handle, &findData) == 0);
        _findclose(handle);
    }

    void ComponentManager::finish() {
        state = State::IDLING;
    }

    ComponentInfo ComponentManager::getActiveComponentInfo() const {
        return activeComponent;
    }

    ComponentManager::State ComponentManager::getState() const {
        return state;
    }

    chrono::duration<double> ComponentManager::getLastExecTime() const {
        return lastEndTime - lastStartTime;
    }

    ComponentManager::~ComponentManager()
    {
        for (auto& h : loadedDlls) {
            ::FreeLibrary(h);
        }
    }

} // namespace NRenderer
