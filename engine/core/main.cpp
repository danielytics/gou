
#include "gou_engine.hpp"
#include "core/config.hpp"
#include "core/engine.hpp"
#include "core/modules.hpp"
#include "utils/clock.hpp"
#include "graphics/graphics.hpp"

#define SPDLOG_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <physfs.hpp>

void setupPhysFS (const char* argv0)
{
    const std::vector<std::string>& sourcePaths = entt::monostate<"game/sources"_hs>{};
    physfs::init(argv0);
    // Mount game sources to search path
    for (auto path : sourcePaths) {
        SPDLOG_DEBUG("Adding to path: {}", path);
        physfs::mount(path, "/", 1);
    }
}

std::shared_ptr<spdlog::logger> setupLogging () {
    std::map<std::string,spdlog::level::level_enum> log_levels{
        {"trace", spdlog::level::trace},
        {"debug", spdlog::level::debug},
        {"info", spdlog::level::info},
        {"warn", spdlog::level::warn},
        {"error", spdlog::level::err},
        {"off", spdlog::level::off},
    };
    const std::string& raw_log_level = entt::monostate<"tools/log-level"_hs>{};
    std::string log_level = raw_log_level;
#ifndef DEBUG_BUILD
    // Only debug builds have debug or trace log levels
    if (log_level == "debug" || log_level == "trace") {
        log_level = "info";
    }
#endif
    // TODO: Error checking for invalid values of log_level
    spdlog::level::level_enum level = log_levels[log_level];
    std::vector<spdlog::sink_ptr> sinks;
    if (level != spdlog::level::off) {
        spdlog::init_thread_pool(8192, 1);
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
        sinks = {stdout_sink};
    }
    auto logger = std::make_shared<spdlog::async_logger>("frenzy", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);
    spdlog::set_default_logger(logger);
    if (level != spdlog::level::off) {
        spdlog::set_pattern("[%D %T.%f] [Thread: %t] [%-8l] %^%v%$");
        spdlog::info("Setting log level to: {}", log_level);
#ifdef DEBUG_BUILD
        // Set global profiling on or off
        // Profile::profiling_enabled = profiling;
#endif
    }
    spdlog::set_level(level);
    return logger;
}

int main (int argc, char* argv[])
{
    // Load base engine settings
    if (! core::readUserConfig(argc, argv)) {
        return -1;
    }
    // Initialise logger and filesystem
    auto logger = setupLogging();
    setupPhysFS(argv[0]);
    // Now that both the filesystem and logger are set up, load the game-specific settings
    if (! core::readGameConfig()) {
        return -1;
    }
    
    // Now start up the engine
    spdlog::info("Initialising");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        spdlog::critical("Failed to initialise SDL");
        return 0;
    }

    // Load game controller mapping
    {
        std::string controllerMapping = helpers::readToString("gamecontrollerdb.txt");
        auto rwop = SDL_RWFromConstMem(controllerMapping.data(), controllerMapping.size());
        defer_calls([&rwop]{SDL_RWclose(rwop);});
        if (SDL_GameControllerAddMappingsFromRW(rwop, 0) < 0) {
            spdlog::critical("Could not read gamepad mapping database.");
            return 0;
        }
    }

    bool clean_exit = true;
    try {
        bool running = true;
        // bool inputLearnMode = false;
        // std::unordered_map<InputKeys::Type, frenzy::events::Event> input_mapping;

        core::Engine engine;
        core::ModuleManager moduleManager(engine);
        graphics::Sync* state_sync;
        struct ImGuiContext* imgui_ctx;
        graphics::Context* graphics_context = graphics::init(engine, state_sync, imgui_ctx);
        if (graphics_context == nullptr || !moduleManager.load(logger, imgui_ctx)) {
            spdlog::critical("Could not load some required modules. Terminating.");
        } else {
            engine.setupSystems(state_sync);
            // Scene scene;
            // input_mapping[InputKeys::make(InputKeys::KeyType::KeyboardButton, SDL_SCANCODE_SPACE)] = frenzy::events::Event{
            //     "character/jump"_event, entt::null, scene.playerEntity(), {0, 0, 0}, 0, 0
            // };
        

            // Initialise timekeeping
            ElapsedTime time_since_start = 0L; // microseconds
            ElapsedTime frame_time_micros = 0L; // microseconds
            auto start_time = Clock::now();
            auto previous_time = start_time;
            auto current_time = start_time;
            uint64_t total_frames = 0;
    #ifdef DEV_MODE
            ElapsedTime last_update_time = 0L; // microseconds
            const ElapsedTime update_interval = entt::monostate<"dev-mode/reload-interval"_hs>();
    #endif

            engine.callModuleHook<gou::api::Module::CallbackMasks::LOAD_SCENE>();

            // Run main loop
            spdlog::info("Game Running...");
            do {
                engine.handleInput(running);

                // // Process previous frames events
                // for (auto& event : utilities::const_iterate(engine.events())) {
                //     switch (event.type) {
                //         case events::animation::Rotated::Type:
                //         {
                //             SPDLOG_DEBUG("Got 'rotated' event from entity {}", event.source);
                //             break;
                //         }
                //         case "input/toggle-learn-mode"_event:
                //         {
                //             inputLearnMode = !inputLearnMode;
                //             break;
                //         }
                //         default:
                //             SPDLOG_DEBUG("Unhandled event: {}", event.type);
                //             break;
                //     }
                // }

                // // Execute systems and copy current frames events for processing next frame
                engine.execute(time_since_start / 1000000.0, frame_time_micros / 1000000.0, total_frames);

                // Update timekeeping
                previous_time = current_time;
                current_time = Clock::now();
                frame_time_micros = std::chrono::duration_cast<std::chrono::microseconds>(current_time - previous_time).count();
                SPDLOG_TRACE("Frame {} took: {} μs", total_frames, frame_time_micros);
                if (frame_time_micros < 1000000L) /* Needs clang 12: [[likely]] */ {
                    // Accumulate time. Do this rather than measuring from the start time so that we can 'omit' time, eg when paused.
                    time_since_start += frame_time_micros;
                } else {
                    // If frame took over a second, assume debugger breakpoint
                    previous_time = current_time;
                }
                ++total_frames;

    #ifdef DEV_MODE
                // In dev mode, update plugins every few seconds for hot code reloading
                if (time_since_start - last_update_time > update_interval) {
                    moduleManager.update();
                }
    #endif

            } while (running);

            auto micros_per_frame = time_since_start / total_frames;
            if (micros_per_frame == 0) {
                micros_per_frame = 1;
            }
            spdlog::info("Average framerate: {:d} ({:.2f}ms per frame)", 1000000 / micros_per_frame, micros_per_frame / 1000.0f);

            // Shut down graphics thread
            graphics::term(graphics_context);

            // Clear data before unloading modules, to avoid referencing memory owned by modules after they are unloaded
            engine.reset();
            moduleManager.unload();
            logger->flush();
        }
    } catch (const std::exception& e) {
        spdlog::error("Uncaught exception: {}", e.what());
        spdlog::critical("Terminating.");
        clean_exit = false;
    }
    SDL_Quit();
    physfs::deinit();
    if (clean_exit) {
        spdlog::info("Goodbye, until next time.");
    }
    return 0;
}

