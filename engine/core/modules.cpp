
#include "gou_engine.hpp"
#include "modules.hpp"
#include "engine.hpp"
#include "constants.hpp"

#include "utils/parser.hpp"

#define CR_HOST
#ifdef DEBUG_BUILD
template <typename... Args>
void cr_debug_log (const std::string& fmt, Args... args) {
    std::string format = fmt;
    const std::string search = "%s";
    const std::string replace = "{}";
    std::string::size_type n = 0;
    while ( ( n = format.find(search, n ) ) != std::string::npos ) {
        format.replace(n, search.size(), replace);
        n += replace.size();
    }
    if (format.back() == '\n') {
        format.pop_back();
    }
    spdlog::debug(format, args...);
}
#define CR_DEBUG
#define CR_TRACE
#define CR_LOG(...)
#define CR_ERROR cr_debug_log
#endif
#include <cr.h>

struct ModuleInfo {
    cr_plugin ctx;
    std::string name;
};

namespace core::detail {
    struct ModuleData {
        std::vector<ModuleInfo> modules;
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

bool core::ModuleManager::load ()
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
                ModuleInfo info;
                info.ctx.userdata = m_engine;

                bool required = toml::find_or<bool>(module_config, "required", false);

                // Build file path and name
                auto filename = path + toml::find<std::string>(module_config, "path");
                if (filename.back() != gou::constants::PathSeparator) {
                    filename += gou::constants::PathSeparator;
                }
                filename += toml::find<std::string>(module_config, "name") + ".module";

                // Load module
                if (cr_plugin_open(info.ctx, filename.c_str())) {
                    info.name = filename;
                    auto result = cr_plugin_update(info.ctx);
                    if (result == 0) {
                        spdlog::info("Loaded module: {}", filename);
                        m_data->modules.push_back(info);
                     } else {
                         cr_plugin_close(info.ctx);
                         if (required) {
                             spdlog::error("Failed to load required module: {} ({})", filename, result);
                             success = false;
                         } else {
                            spdlog::warn("Failed to load module: {} ({})", filename, result);
                         }
                     }
                } else {
                    spdlog::error("Failed to load module: {}", filename);
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
    for (auto& info : m_data->modules) {
        cr_plugin_update(info.ctx);
    }
}

void core::ModuleManager::unload ()
{
    for (auto& info : helpers::reverse(m_data->modules)) {
        spdlog::info("Unloading module: {}", info.name);
        cr_plugin_close(info.ctx);
    }
    m_data->modules.clear();
}