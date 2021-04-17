
#include <gou.hpp>

struct velocity {
    double dx;
    double dy;
};

class TestModule2 : public gou::Module<TestModule2> {
    GOU_CLASS(TestModule2)
public:

    void onLoad (gou::Engine e)
    {
        auto& engine = e.engine;
        auto& registry = engine.registry();
        registry.view<components::Position, velocity>().each([](components::Position &pos, velocity &vel) {
            pos.z = static_cast<int>(vel.dx) + static_cast<int>(vel.dy);
        });
    }
};

GOU_MODULE(TestModule2)
