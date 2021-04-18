
#include "scenes.hpp"
#include "utils/parser.hpp"
#include "core/engine.hpp"
#include <physfs.hpp>

world::SceneManager::SceneManager (core::Engine& engine) :
    m_engine(engine)
{

}

world::SceneManager::~SceneManager ()
{

}

void world::SceneManager::loadSceneList (const std::string& filename)
{
    // Clear previous scenes, if any
    m_scenes.clear();

    // Load new scenes
    const auto config = parser::parse_toml(filename);
    if (config.contains("scenes")) {
        const auto& scenes = config.at("scenes");
        for (const auto& [name, path]  : scenes.as_table()) {
            auto filename = path.as_string();
            if (physfs::exists(filename)) {
                m_scenes[entt::hashed_string{name.c_str()}.value()] = filename;
            } else {
                spdlog::warn("Scene \"{}\" file does not exist: {}", name, filename);
            }
            
        }
    }
}

void world::SceneManager::loadScene (entt::hashed_string scene)
{
    using CM = gou::api::Module::CallbackMasks;

    auto it = m_scenes.find(scene.value());
    if (it != m_scenes.end()) {
        auto& registry = m_engine.registry();

        // Unload previous scene, if there is one
        if (m_current_scene != entt::hashed_string{}) {
            spdlog::info("[SceneManager] Unloading scene: {}", m_current_scene.data());
            m_engine.callModuleHook<CM::UNLOAD_SCENE>();
            // Destroy all entities that aren't marked as global
            registry.each([&registry](auto entity){
                if (! registry.all_of<components::Global>(entity)) {
                    registry.destroy(entity);
                }
            });
        }
        spdlog::info("[SceneManager] Loading scene: {}", scene.data());
        const auto config = parser::parse_toml(it->second);

        if (config.contains("entity")) {
            for (const auto& entity : config.at("entity").as_array()) {
                auto e = registry.create();
                SPDLOG_TRACE("[SceneManager] Creating new entity: {}", entt::to_integral(e));
                for (const auto& [name_str, component]  : entity.as_table()) {
                    SPDLOG_TRACE("[SceneManager] Adding component to entity {}: {}", entt::to_integral(e), name_str);
                    toml::value value = component;
                    m_engine.loadComponent(entt::hashed_string{name_str.c_str()}, e, reinterpret_cast<const void*>(&value));
                }
            }
        }

        m_current_scene = scene;
        m_engine.callModuleHook<CM::LOAD_SCENE>(scene);
    } else {
        spdlog::error("[SceneManager] Could not load scene because it does not exist: {}", scene.data());
    }
}
