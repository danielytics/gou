
#include "gou_engine.hpp"
#include "modules.hpp"
#include "engine.hpp"
#include "constants.hpp"

#include "utils/parser.hpp"
#include "utils/helpers.hpp"

#define CR_HOST
#ifdef DEBUG_BUILD
template <typename... Args>
void cr_debug_log (const std::string& fmt, Args... args) {
    std::string format = fmt;
    helpers::string_replace_inplace(format, "%s", "{}");
    helpers::string_replace_inplace(format, "%d", "{}");
    spdlog::debug(std::string{"cr: "} + format, args...);
}
#define CR_DEBUG
#define CR_TRACE
#define CR_LOG(...)
#define CR_ERROR cr_debug_log
#endif
#include <cr.h>

namespace core::detail {
    struct ModuleData {
        std::vector<cr_plugin> modules;
    };
}

core::ModuleManager::ModuleManager(core::Engine& engine) : m_engine(&engine)
{
    m_data = new core::detail::ModuleData;
}

core::ModuleManager::~ModuleManager()
{
    delete m_data;
}

bool core::ModuleManager::load (std::shared_ptr<spdlog::logger> logger, ImGuiContext* imgui_context)
{
    bool success = true;

    std::string path = entt::monostate<"game/modules-path"_hs>();
    if (path != "" && path.back() != gou::constants::PathSeparator) {
        path += gou::constants::PathSeparator;
    }
    const std::string& modules = entt::monostate<"game/modules-file"_hs>();
    spdlog::info("Reading modules from: {}", modules);
    const auto config = parser::parse_toml(modules);
    if (config.contains("module")) {
        const auto& module_list = toml::find<TomlArray>(config, "module");
        for (const auto& module_config : module_list) {
            if (toml::find_or<bool>(module_config, "enabled", false) && module_config.contains("path") && module_config.contains("name")) {
                auto info = new gou::api::detail::ModuleInfo{
                    toml::find<std::string>(module_config, "name"),
                    m_engine,
                    logger,
                    imgui_context,
                };

                bool required = toml::find_or<bool>(module_config, "required", false);

                // Build file path and name
                auto filename = path + toml::find<std::string>(module_config, "path");
                if (filename.back() != gou::constants::PathSeparator) {
                    filename += gou::constants::PathSeparator;
                }
                filename += info->name + ".module";

                // Load module
                spdlog::info("Loading module \"{}\" from: {}", info->name, filename);
                cr_plugin ctx;
                ctx.userdata = info;
                if (cr_plugin_open(ctx, filename.c_str())) {
                    auto result = cr_plugin_update(ctx);
                    if (result == 0) {
                        spdlog::info("Loaded module \"{}\"", info->name);
                        m_data->modules.push_back(ctx);
                     } else {
                         cr_plugin_close(ctx);
                         if (required) {
                             spdlog::error("Failed to load required module \"{}\": {}", info->name, result);
                             success = false;
                         } else {
                            spdlog::warn("Failed to load module \"{}\" {}", info->name, result);
                         }
                     }
                } else {
                    spdlog::error("Could not open module: {}", filename);
                }
            }
        }   
    } else {
        SPDLOG_DEBUG("No 'module' list found in {}", modules);
    }

    if (! success) {
        unload();
    }

    return success;
}

void core::ModuleManager::update ()
{
    for (auto& ctx : m_data->modules) {
        cr_plugin_update(ctx);
    }
}

void core::ModuleManager::unload ()
{
    // Unload each plugin in reverse load order
    for (auto& ctx : helpers::reverse(m_data->modules)) {
        auto info = static_cast<gou::api::detail::ModuleInfo*>(ctx.userdata);
        spdlog::info("Unloading module \"{}\"", info->name);
        cr_plugin_close(ctx);
        delete info;
    }

    // Make sure the list of modules is empty to prevent accidental use of deleted data
    m_data->modules.clear();
}
