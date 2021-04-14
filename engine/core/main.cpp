
#include "gou_engine.hpp"
#include "core/config.hpp"
#include "core/engine.hpp"
#include "core/modules.hpp"
#include "utils/clock.hpp"

#define SPDLOG_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <physfs.hpp>

#include <SDL.h>


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
#ifdef DEBUG_BUILD
        {"trace", spdlog::level::trace},
        {"debug", spdlog::level::debug},
#endif
        {"info", spdlog::level::info},
        {"warn", spdlog::level::warn},
        {"error", spdlog::level::err},
        {"off", spdlog::level::off},
    };
    const std::string& log_level = entt::monostate<"tools/log-level"_hs>{};
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

    // auto& registry = engine.registry();

    // for(auto i = 0; i < 3; ++i) {
    //     registry.emplace<position>(registry.create(), i, i, 0);
    // }

    // std::vector<std::string> modules{
    //     "modules/test.module",
    //     "modules/test2.module",
    // };
    // moduleManager.load(modules);

    // registry.view<position>().each([](auto entity, auto &position) {
    //     std::cout << "(" << position.x << ", " << position.y << ", " << position.z << "): " << static_cast<int>(entt::to_integral(entity) + 16u) << "\n";
    // });

    // registry = {};
    
    // moduleManager.unload();

    try {
        bool running = true;
        SDL_Event event;
        SDL_GameController* gameController;
        // bool inputLearnMode = false;
        // std::unordered_map<InputKeys::Type, frenzy::events::Event> input_mapping;

        core::Engine engine;
        // engine.setup(logger);
        core::ModuleManager moduleManager(engine);
        moduleManager.load();

        // Scene scene;
        // input_mapping[InputKeys::make(InputKeys::KeyType::KeyboardButton, SDL_SCANCODE_SPACE)] = frenzy::events::Event{
        //     "character/jump"_event, entt::null, scene.playerEntity(), {0, 0, 0}, 0, 0
        // };
        
        // Barrier barrier; // TODO: Replace with std::barrier when support is more widely available
        // moodycamel::ConcurrentQueue<SDL_Event> eventQueue;
        // graphics::Context* graphics_context = graphics::init(barrier, eventQueue, engine);

        // barrier.init(Barrier::Which::Updating);

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

        // Run main loop
        spdlog::info("Game Running...");
        do {
            // Gather and dispatch input
            while (SDL_PollEvent(&event))
            {
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                    {
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            running = false;
                        } else {
                            // handleInput(engine, input_mapping, InputKeys::KeyType::KeyboardButton, event.key.keysym.scancode, [&event]() -> float {
                            //     return event.key.state == SDL_PRESSED ? 1.0f : 0.0f;
                            // });
                        }
                        break;
                    }
                    case SDL_CONTROLLERDEVICEADDED:
                        gameController = SDL_GameControllerOpen(event.cdevice.which);
                        if (gameController == 0) {
                            spdlog::error("Could not open gamepad {}: {}", event.cdevice.which, SDL_GetError());
                        } else {
                            spdlog::info("Gamepad detected {}", SDL_GameControllerName(gameController));
                        }
                        break;
                    case SDL_CONTROLLERDEVICEREMOVED:
                        SDL_GameControllerClose(gameController);
                        break;
                    case SDL_CONTROLLERBUTTONDOWN:
                    case SDL_CONTROLLERBUTTONUP:
                    {
                        // handleInput(engine, input_mapping, InputKeys::KeyType::ControllerButton, event.cbutton.button, [&event]() -> float {
                        //     return event.cbutton.state ? 1.0f : 0.0f;
                        // });
                        break;
                    }
                    case SDL_CONTROLLERAXISMOTION:
                    {
                        // handleInput(engine, input_mapping, InputKeys::KeyType::ControllerAxis, event.caxis.axis, [&event]() -> float {
                        //     float value = float(event.caxis.value) * Constants::GamePadAxisScaleFactor;
                        //     if (std::fabs(value) < 0.15f /* TODO: configurable deadzones */) {
                        //         return 0;
                        //     }
                        //     return value;
                        // });
                        break;
                    }
                    default:
                        break;
                };
                // Send to event queue for render thread to access (used by Dear ImGui)
                // eventQueue.enqueue(event);
            }

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
            // engine.execute(time_since_start / 1000000.0, frame_time_micros / 1000000.0, total_frames);

            // // Handoff render data to renderer
            // // ...handoff...
            // // Sync after updating to ensure that an update cycle is complete for the renderer to render
            // barrier.sync(Barrier::Which::Updating);

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
        logger->flush();

        auto micros_per_frame = time_since_start / total_frames;
        if (micros_per_frame == 0) {
            micros_per_frame = 1;
        }
        spdlog::info("Average framerate: {:d} ({:.2f}ms per frame)", 1000000 / micros_per_frame, micros_per_frame / 1000.0f);

        // graphics::term(graphics_context);
        moduleManager.unload();
    } catch (const std::exception& e) {
        spdlog::error("Uncaught exception: {}", e.what());
        spdlog::critical("Terminating.");
    }
    SDL_Quit();
    physfs::deinit();
    spdlog::info("Goodbye, until next time.");
    return 0;
}

