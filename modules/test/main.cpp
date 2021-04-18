
#include <gou.hpp>
#include <imgui.h>

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

    void onAfterRender (gou::Renderer& r)
    {
        static float f = 0.0f;
        static int counter = 0;
        static ImVec4 clear_color{0.f, 0.f, 0.f, 1.0f};

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    void onLoadScene (gou::Scene& scene)
    {

    }
};

GOU_MODULE(TestModule)
