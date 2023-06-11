#include "templates/MaterialTemplates.hpp"

using namespace NRenderer;
using namespace std;

unordered_map<int, MaterialTemplates::Template> MaterialTemplates::templates{};
MaterialTemplates::InitTemplates MaterialTemplates::initTemplates = MaterialTemplates::InitTemplates{MaterialTemplates::templates};