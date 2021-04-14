
#include "gou_engine.hpp"
#include "config.hpp"
#include "utils/parser.hpp"

#include <cxxopts.hpp>

// Only set if table contains key and type conversion passes
template <entt::id_type ID, typename T> void maybe_set (const TomlValue& table, const std::string& key) {
    if (table.contains(key)) {
        const auto value = toml::expect<T>(table.at(key));
        if (value.is_ok()) {
            entt::monostate<ID>{} = value.unwrap();
        }   
    }
}

bool core::readUserConfig (int argc, char* argv[])
{
    cxxopts::Options options("Frenzy", "Game Engine");
    options.add_options()
#ifdef DEBUG_BUILD
        ("d,debug", "Enable debug rendering")
        ("p,profiling", "Enable profiling")
#endif
        ("l,loglevel", "Log level", cxxopts::value<std::string>())
        ("g,gamefiles", "Path(s) to game files", cxxopts::value<std::vector<std::string>>())
        ("m,modules", "Modules list file", cxxopts::value<std::string>())
        ("modulepath", "Path to Module files", cxxopts::value<std::string>())
        ("i,init", "Initialisation file", cxxopts::value<std::string>()->default_value("init.toml"));
    auto result = options.parse(argc, argv);

    //******************************************************//
    //                                                      //
    // Base settings loaded from init TOML file             //
    // These settings are meant to be editable by players   //
    // Loaded from Filesystem (not PhysicsFS)               //
    //                                                      //
    //******************************************************//
    try {
        const auto config = parser::parse_toml(result["init"].as<std::string>(), parser::FileLocation::FileSystem);

        //******************************************************//
        // TELEMETRY
        //******************************************************//
        if (result["loglevel"].count() == 0 ) {
            if (config.contains("telemetry")) {
                const auto& telemetry = config.at("telemetry");
                if (telemetry.contains("logging")) {
                    maybe_set<"tools/log-level"_hs, std::string>(telemetry, "logging");
                }
            } else {
                entt::monostate<"tools/log-level"_hs>{} = "info";
            }
        } else {
            entt::monostate<"tools/log-level"_hs>{} = result["loglevel"].as<std::string>();
        }

        //******************************************************//
        // GAME
        //******************************************************//
        // Set default settings for [game] section
        entt::monostate<"game/sources"_hs>{} = std::vector<std::string>{"common"};
        // Overwrite with settings
        if (config.contains("game")) {
            const auto& game = config.at("game");
            if (game.contains("sources")) {
                const auto& sources = game.at("sources").as_array();
                std::vector<std::string> source_files;
                for (const auto& source : sources) {
                    source_files.push_back(source.as_string());
                }
                entt::monostate<"game/sources"_hs>{} = source_files;
            }

            entt::monostate<"game/modules-file"_hs>{} = toml::find_or<std::string>(game, "modules", "modules.toml");
            maybe_set<"game/config-file"_hs, std::string>(game, "config");
        } else {
            entt::monostate<"game/modules-file"_hs>{} = std::string{"modules.toml"};
            entt::monostate<"game/config-file"_hs>{} = std::string{"game.toml"};
        }
        // Override game sources, if specified on commandline
        if (result["gamefiles"].count() > 0) {
            entt::monostate<"game/sources"_hs>{} = result["gamefiles"].as<std::vector<std::string>>();;
        }
        // Override module list file, if specified on commandline
        if (result["modules"].count() > 0) {
            entt::monostate<"game/modules-file"_hs>{} = result["modules"].as<std::string>();;
        }
        // Set module path, if specified on commandline (modules loaded relative to this path)
        if (result["modulepath"].count() > 0) {
            entt::monostate<"game/modules-path"_hs>{} = result["modulepath"].as<std::string>();;
        } else {
            entt::monostate<"game/modules-path"_hs>{} = "";
        }

        //******************************************************//
        // GRAPHICS
        //******************************************************//
        if (config.contains("graphics")) {
            const auto& graphics = config.at("graphics");
            int width = 640;
            int height = 480;
            if (graphics.contains("resolution")) {
                std::string empty_string;
                const auto& resolution = toml::find_or<std::string>(graphics, "resolution", empty_string);
                if (resolution == "720p") {
                    width = 1280;
                    height = 720;
                } else if (resolution == "1080p") {
                    width = 1920;
                    height = 1080;
                } else if (resolution == "1440p") {
                    width = 2560;
                    height = 1440;
                } else if (resolution == "4k") {
                    width = 4096;
                    height = 2160;
                } else {
                    // parse "<width>x<height>"
                    auto idx = resolution.find('x');
                    if (idx != std::string::npos) {
                        auto w = resolution.substr(0, idx);
                        auto h = resolution.substr(idx+1, resolution.size());
                        width = std::stoi(w);
                        height = std::stoi(h);
                    }
                }
            }
            entt::monostate<"graphics/resolution/width"_hs>{} = width;
            entt::monostate<"graphics/resolution/height"_hs>{} = height;
            entt::monostate<"graphics/fullscreen"_hs>{} = toml::find_or<bool>(graphics, "fullscreen", false);
            entt::monostate<"graphics/v-sync"_hs>{} = toml::find_or<bool>(graphics, "vsync", true);
            entt::monostate<"graphics/field-of-view"_hs>{} = toml::find_or<float>(graphics, "fov", 60.0f);
            entt::monostate<"graphics/debug-rendering"_hs>{} = toml::find_or<bool>(graphics, "debug", false);
        } else {
            // No [graphics] section, use default settings
            entt::monostate<"graphics/resolution/width"_hs>{} = int{640};
            entt::monostate<"graphics/resolution/height"_hs>{} = int{480};
            entt::monostate<"graphics/fullscreen"_hs>{} = false;
            entt::monostate<"graphics/v-sync"_hs>{} = true;
            entt::monostate<"graphics/field-of-view"_hs>{} = 60.0f;
            entt::monostate<"graphics/debug-rendering"_hs>{} = false;
        }
#ifdef DEBUG_BUILD
        entt::monostate<"graphics/debug-rendering"_hs>{} = bool{result["debug"].count() > 0};
#endif

        //******************************************************//
        // DEVELOPMENT MODE
        //******************************************************//
#ifdef DEV_MODE
        if (config.contains("dev-mode")) {
            const auto& devmode = config.at("dev-mode");
            entt::monostate<"dev-mode/enabled"_hs>{} = toml::find_or<bool>(devmode, "enabled", false);
            entt::monostate<"dev-mode/reload-interval"_hs>{} = ElapsedTime(toml::find_or<float>(devmode, "reload-interval", 5.0f) * 1000000L);
        } else {
            entt::monostate<"dev-mode/enabled"_hs>{} = bool{false};
        }
#endif

    } catch (const std::exception& e) {
        spdlog::critical("Could not load settings: {}", e.what());
        return false;
    }

    return true;
}

bool core::readGameConfig () {
    //******************************************************//
    //                                                      //
    // Settings loaded from game config                     //
    // These settings are NOT meant to be edited by players //
    // Loaded from PhysicsFS                                //
    //                                                      //
    //******************************************************//
    try {
        const std::string& game_config = entt::monostate<"game/config-file"_hs>();
        const auto config = parser::parse_toml(game_config);
        
        //******************************************************//
        // MEMORY
        //******************************************************//
        // Default settings for [memory] section, use default settings
        entt::monostate<"memory/events/pool-size"_hs>{} = std::uint32_t{96};
        // Overwrite with settings
        if (config.contains("memory")) {
            const auto& memory = config.at("memory");
            if (memory.contains("events")) {
                maybe_set<"memory/events/pool-size"_hs, std::uint32_t>(memory.at("events"), "per-thread-pool-size");
            }
        } else {
            
        }

        //******************************************************//
        // PHYSICS
        //******************************************************//
        if (config.contains("physics")) {
            const auto& physics = config.at("physics");
            entt::monostate<"physics/max-substeps"_hs>{} = toml::find_or<int>(physics, "max-substeps", 5);
            // Gravity
            float gx = 0;
            float gy = 0;
            float gz = 0;
            if (physics.contains("gravity")) {
                const auto& gravity = physics.at("gravity");
                gx = toml::find_or<double>(gravity, "x", 0);
                gy = toml::find_or<double>(gravity, "y", 0);
                gz = toml::find_or<double>(gravity, "z", 0);
            }
            entt::monostate<"physics/gravity"_hs>{} = glm::vec3{gx, gy, gz};
            entt::monostate<"physics/time-step"_hs>{} = float(1.0 / toml::find_or<double>(physics, "target-framerate", 30.0));
        } else {
            // No [physics] section, use default settings
            entt::monostate<"physics/time-step"_hs>{} = float(1.0f / 30.0f);
            entt::monostate<"physics/max-substeps"_hs>{} = int(5);
            entt::monostate<"physics/gravity"_hs>{} = glm::vec3{0, 0, 0};
        }
    } catch (const std::exception& e) {
        spdlog::critical("Could not load game config: {}", e.what());
        return false;
    }

    return true;
}
