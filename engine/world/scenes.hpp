#pragma once

#include <gou_engine.hpp>

namespace core
{
    class Engine;
} // core::


namespace world {

    class SceneManager {
    public:
        SceneManager (core::Engine&);
        ~SceneManager ();

        void loadSceneList (const std::string& filename);
        void loadScene (entt::hashed_string scene);

    private:
        core::Engine& m_engine;
        spp::sparse_hash_map<entt::hashed_string::hash_type, std::string, helpers::Identity> m_scenes;
        entt::hashed_string m_current_scene;
    };

} // world::
