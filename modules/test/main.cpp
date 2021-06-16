
#include <gou/gou.hpp>
#include <imgui.h>

class TestModule : public gou::Module<TestModule> {
    GOU_MODULE_CLASS(TestModule)
public:

    void onLoad (gou::Engine e)
    {
        info("Load");
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

#ifndef WITHOUT_IMGUI
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
#endif
    }

    void onBeforeUpdate (gou::Scene& scene) {
        scene.registry().view<components::Transform>().each([&scene](auto entity, auto& transform){
            transform.rotation.z += 0.5f * scene.deltaTime(); // Half a rotation per second
        });
    }

    void onLoadScene (gou::Scene& scene)
    {
        {
            auto entity = scene.create("test-entity"_hs);
            scene.add<components::Position>(entity, glm::vec3{1.0f, 2.0f, 3.0f});
        }
        {
            auto entity = scene.find("test1"_hs);
            if (entity == entt::null) {
                warn("Entity 'test1' not found.");
            } else {
                const auto& p = scene.get<components::Position>(entity);
                warn("Entity 'test1' has position: ({}, {}, {})", p.point.x, p.point.y, p.point.z);
            }
        }

    }
};

GOU_MODULE(TestModule)
