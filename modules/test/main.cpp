
#include <gou.hpp>

struct velocity {
    double dx;
    double dy;
};

class TestModule : public gou::Module<TestModule> {
    GOU_CLASS(TestModule)
public:

    void onLoad (gou::Engine e)
    {
        info("Load");
        auto& engine = e.engine;
        auto& registry = engine.registry();

        registry.prepare<velocity>();

        const auto view = registry.view<components::Position>();
        registry.insert(view.begin(), view.end(), velocity{1., 2.});

        const auto vel = registry.view<velocity>();

        registry.view<components::Position, velocity>().each([](components::Position &pos, velocity &vel) {
            pos.x += static_cast<int>(16 * vel.dx);
            pos.y += static_cast<int>(16 * vel.dy);
        });
    }

    void onUnload (gou::Engine e)
    {
        info("Unload");
    }

    void onLoadScene (gou::Scene& scene)
    {

    }
};

GOU_MODULE(TestModule)
