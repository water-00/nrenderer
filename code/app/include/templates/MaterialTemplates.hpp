#ifndef __NR_MATERIAL_TEMPLATES_HPP__
#define __NR_MATERIAL_TEMPLATES_HPP__

#include <unordered_map>

#include "scene/Material.hpp"
#include "common/macros.hpp"

namespace NRenderer
{
    class DLL_EXPORT MaterialTemplates
    {
    public:
        struct Template
        {
            unsigned int index;
            string key;
            vector<Property> props;
        };

        static unordered_map<int, Template> templates;

    private:
        struct InitTemplates
        {
            template<typename T>
            void PushProp(Template& p, T&& arg) {
                p.props.push_back(arg);
            }
            template<typename T, typename ...Args>
            void PushProp(Template& p, T&& arg1, Args&& ...args) {
                p.props.push_back(arg1);
                PushProp(p, args...);
            }
            template<typename ...Args>
            void Add(unordered_map<int, Template>& templates, unsigned int index, const string& name, Args&& ...args) {
                templates[index] = {
                    index, name, {}
                };
                PushProp(templates[index], args...);
            }
            InitTemplates(unordered_map<int, Template>& templates) {
                using PW = Property::Wrapper;
                Add(templates, 0, string("Lambertian"),
                    Property("diffuseColor", PW::RGBType{}),
                    Property("diffuseMap", PW::TextureIdType{})
                );
                Add(templates, 1, string("Phong"),
                    Property("diffuseColor", PW::RGBType{}),
                    Property("diffuseMap", PW::TextureIdType{}),
                    Property("specularColor", PW::RGBType{}),
                    Property("specularEx", PW::FloatType{})
                );
                Add(templates, 2, string("Dielectric"),
                    Property("ior", PW::FloatType{float(1.0)}),
                    Property("absorbed", PW::RGBType{RGB{1, 1, 1}})
                );
                Add(templates, 3, string("Conductor"),
                    Property("reflect", PW::RGBType{RGB{1, 1, 1}})
                );
                Add(templates, 4, string("Plastic"),
                    Property("diffuseColor", PW::RGBType{}),
                    Property("specularColor", PW::RGBType{}),
                    Property("refractIndex", PW::FloatType{})
                );
                
            }
        };
        static InitTemplates initTemplates;
    };
}

#endif