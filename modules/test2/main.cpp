
#include <gou/gou.hpp>

class TestModule2 : public gou::Module<TestModule2> {
    GOU_MODULE_CLASS(TestModule2)
public:
    void onLoad (gou::Engine e)
    {
        info("sizeof(Event): {}", sizeof(gou::events::Event));
    }

    void onLoadScene (gou::Scene& scene)
    {
        info("Loading scene {}", scene.name());
        auto entity = scene.find("test-entity"_hs);
        if (entity != entt::null) {
            const auto& position = scene.get<components::Position>(entity);
            info("Entity position = ({}, {}, {})", position.point.x, position.point.y, position.point.z);
        }
    }

    void onUnloadScene (gou::Scene& scene)
    {
        info("Unloading scene {} {}", scene.name(), scene.id() == "test"_hs);
    }
};

GOU_MODULE(TestModule2)
