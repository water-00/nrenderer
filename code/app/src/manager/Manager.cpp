#include "manager/Manager.hpp"
#include "server/Server.hpp"
namespace NRenderer
{
    Manager::Manager()
        : renderSettingsManager             ()
        , assetManager                      ()
        , componentManager                  ()
    {
        componentManager.init(".\\components\\*.dll");
   }
}