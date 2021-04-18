
#include <gou.hpp>

class TestModule2 : public gou::Module<TestModule2> {
    GOU_CLASS(TestModule2)
public:

    void onLoad (gou::Engine e)
    {

    }

    void onLoadScene (gou::Scene& scene)
    {
        auto entity = scene.find("test-entity"_hs);
        if (entity != entt::null) {
            const auto& position = scene.get<components::Position>(entity);
            info("Entity position = ({}, {}, {})", position.x, position.y, position.z);
        }
    }
};

GOU_MODULE(TestModule2)
