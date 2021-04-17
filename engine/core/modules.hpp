#pragma once

#include <spdlog/logger.h>

namespace core {
    namespace detail {
        struct ModuleData;
    }
    /**
     * ModuleManager is the API through which modules are loaded into the engine.
     * Modules then register functionality with the Engine, after which runtime systems will call the correct callbacks at runtime.
     */
    class ModuleManager {
    public:
        ModuleManager(class Engine&);
        ~ModuleManager();

        bool load (std::shared_ptr<spdlog::logger> logger);
        void update ();
        void unload ();

    private:
        class Engine* const m_engine;
        detail::ModuleData* m_data;
    };

}