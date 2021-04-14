
#include <gou.hpp>
#include <iostream>

struct velocity {
    double dx;
    double dy;
};

class TestModule : public gou::Module<TestModule> {
    GOU_CLASS(TestModule)
public:

    void onLoad (gou::Engine e)
    {
        auto& engine = e.engine;
        auto& registry = engine.registry();

        registry.prepare<velocity>();

        const auto view = registry.view<position>();
        registry.insert(view.begin(), view.end(), velocity{1., 2.});

        const auto vel = registry.view<velocity>();

        registry.view<position, velocity>().each([](position &pos, velocity &vel) {
            pos.x += static_cast<int>(16 * vel.dx);
            pos.y += static_cast<int>(16 * vel.dy);
        });
    }
};

GOU_MODULE(TestModule)
