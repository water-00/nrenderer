#include "gtest/gtest.h"
#include "scene/Material.hpp"

using namespace NRenderer;

NRenderer::Material material{};
using PW = Property::Wrapper;
class MaterialTest : public ::testing::Test
{
public:
    MaterialTest() = default;
    ~MaterialTest() = default;
    void SetUp() override {
        material.registerProperty("diffuseColor", PW::RGBType{RGB{1}});
        material.registerProperty("diffuseColor", PW::RGBType{RGB{2}});
    }
    void TearDown() override {

    }
};

using namespace NRenderer;

TEST_F(MaterialTest, MaterialTest) {
    EXPECT_EQ(material.properties.size(), 1);
}