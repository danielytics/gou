
#include "graphics.hpp"
#include "render_api.hpp"
#include "core/engine.hpp"

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <atomic>

#include "renderer.hpp"


namespace imgui {
    void initTheme ();
}

#ifdef DEBUG_BUILD
void GLAPIENTRY opengl_messageCallback (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::string source_string;
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            source_string = "api";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_string = "window_system";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_string = "shader_compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_string = "third_party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            source_string = "application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
        default:
            source_string = "UNKNOWN";
            break;
    }

    std::string type_string;
	switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_string = "error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_string = "deprecated_behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_string = "undefined_behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_string = "portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_string = "performance";
            break;
        case GL_DEBUG_TYPE_OTHER:
        default:
            type_string = "other";
            break;
	}

    std::string severity_string = "unknown";
	switch (severity){
        case GL_DEBUG_SEVERITY_LOW:
            severity_string = "low";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_string = "medium";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            severity_string = "high";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_string = "notification";
            break;
	}

    #define OPENGL_MESSAGE "OpenGL Message (id={:#x}, type={}, source={}, severity={}): {}", id, type_string, source_string, severity_string, message
    if (type == GL_DEBUG_TYPE_ERROR) {
        spdlog::error(OPENGL_MESSAGE);
    } else {
        spdlog::debug(OPENGL_MESSAGE);
    }
}
#endif

int render (void* data);

gou::api::Renderer* graphics::init (core::Engine& engine, graphics::Sync*& state_sync, ImGuiContext*& imgui_context) {
    // Set the OpenGL attributes for our context
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on OS X
    #define OGL_MAJOR_VERSION 4
    #define OGL_MINOR_VERSION 1
#else
    #define OGL_MAJOR_VERSION 4
    #define OGL_MINOR_VERSION 6
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OGL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OGL_MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, int(entt::monostate<"graphics/opengl/minimum-red-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, int(entt::monostate<"graphics/opengl/minimum-green-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, int(entt::monostate<"graphics/opengl/minimum-blue-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, int(entt::monostate<"graphics/opengl/minimum-alpha-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, int(entt::monostate<"graphics/opengl/minimum-framebuffer-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, int(entt::monostate<"graphics/opengl/minimum-depthbuffer-bits"_hs>()));
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, bool(entt::monostate<"graphics/opengl/double-buffered"_hs>()) ? 1 : 0);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
#ifdef DEBUG_BUILD
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    bool fullscreen = entt::monostate<"graphics/fullscreen"_hs>();

    const int width = entt::monostate<"graphics/resolution/width"_hs>();
    const int height = entt::monostate<"graphics/resolution/height"_hs>();
    const bool resizable = entt::monostate<"graphics/resolution/resizable"_hs>();

    // Create renderer and context to share with render thread and graphics API. Context is opague to the engine
    auto renderer = new graphics::RenderAPI(engine);
    renderer->window = SDL_CreateWindow(
        std::string(entt::monostate<"graphics/window/title"_hs>()).c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : (resizable ? SDL_WINDOW_RESIZABLE : 0)));

    renderer->gl_render_context = SDL_GL_CreateContext(renderer->window);
    renderer->gl_init_context = SDL_GL_CreateContext(renderer->window);
    SDL_GL_MakeCurrent(renderer->window, renderer->gl_init_context);

    SDL_GL_SetSwapInterval(bool(entt::monostate<"graphics/v-sync"_hs>()) ? 1 : 0);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        spdlog::critical("Failed to initialize GLAD");
        return nullptr;
    }

#ifdef DEBUG_BUILD
    int max_tex_layers, max_combined_tex, max_vert_tex, max_geom_tex, max_frag_tex, max_tex_size;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_tex_layers);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_combined_tex);
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_vert_tex);
    glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &max_geom_tex);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_frag_tex);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
    spdlog::debug("Texture limits: {} combined units, {}/{}/{} vs/gs/fs units, {} array layers, {}x{} max size", max_combined_tex, max_vert_tex, max_geom_tex, max_frag_tex, max_tex_layers, max_tex_size, max_tex_size);
    int max_vert_uniform_vec, max_frag_uniform_vec, max_varying_vec, max_vertex_attribs;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_vert_uniform_vec);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_frag_uniform_vec);
    glGetIntegerv(GL_MAX_VARYING_VECTORS, &max_varying_vec);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    spdlog::debug("Shader limits: {} vertex attributes, {} varying vectors, {} vertex vectors, {} fragment vectors", max_vertex_attribs, max_varying_vec, max_vert_uniform_vec, max_frag_uniform_vec);
#endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();

    // Setup window
    renderer->windowChanged();

    // Share sync object with engine
    state_sync = &renderer->state_sync;

    // Start render thread
    renderer->render_thread = SDL_CreateThread(render, "render", reinterpret_cast<void*>(renderer));

    return renderer;
}

int render (void* data) {
    using CM = gou::api::Module::CallbackMasks;

    try {
        graphics::RenderAPI* render_api = static_cast<graphics::RenderAPI*>(data);
        SDL_GL_MakeCurrent(render_api->window, render_api->gl_render_context);

#ifdef DEBUG_BUILD
        spdlog::info("Setting error callback");
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_messageCallback, 0);
        GLuint unused_ids = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unused_ids, true);
#endif

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);
        glClearDepth(1.0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable multiple windows
        imgui::initTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(render_api->window, render_api->gl_render_context);
        ImGui_ImplOpenGL3_Init("#version 450");

        // Get context data
        auto& running = render_api->running;
        auto& state_sync = render_api->state_sync;
        auto& cv = state_sync.sync_cv;
        auto& engine = render_api->engine;

        glm::mat4 projection_matrix;
        glm::vec4 viewport;
        std::vector<Sprite> sprites;
        
        // Sync with the engine, so that it knows the render thread is set up
        {
            // Wait to grab the lock
            std::unique_lock<std::mutex> lock(state_sync.state_mutex);
            cv.wait(lock, [&state_sync](){ return state_sync.owner == graphics::Sync::Owner::Renderer; });
            // And release it again right away
            state_sync.owner = graphics::Sync::Owner::Engine;
            lock.unlock();
            cv.notify_one();
        }

        init();
        
        spdlog::info("Render thread running");
        // Run render loop
        do {
            EASY_BLOCK("Rendering frame", profiler::colors::Teal100);

            /*********************************************************************/
            /* Wait for engine to hand over exclusive access to engine state.
            * Renderer will then access the ECS registry to gather all components needed for rengering,
            * accumulate a render list and hand exclusive access back to the engine. The renderer will
            * then asynchronously render from its locally owned render list.
            *********************************************************************/
            {
                EASY_BLOCK("Renderer waiting for exclusive access to engine", profiler::colors::Red100);
                // Wait for exclusive access to engine state
                std::unique_lock<std::mutex> lock(state_sync.state_mutex);
                {
                    cv.wait(lock, [&state_sync](){ return state_sync.owner == graphics::Sync::Owner::Renderer; });
                }
                EASY_END_BLOCK;

                // Let Dear ImGui process events from event queue
                for (const auto& event : engine.inputEvents()) {
                    ImGui_ImplSDL2_ProcessEvent(&event);
                }

                EASY_BLOCK("Collecting render data", profiler::colors::Red300);
                
                // Call onPrepareRender during the critical section, before performing any rendering
                engine.callModuleHook<CM::PREPARE_RENDER>();

                // Gather render data into render list
                entt::registry& registry = render_api->engine.registry(gou::api::Registry::Runtime);

                sprites.clear();
                registry.view<components::graphics::Sprite, components::Position>(entt::exclude<components::Transform>).each([&sprites](const auto, const auto& position) {
                    sprites.emplace_back(Sprite{position.point, glm::vec3(0.0f), glm::vec3(1.0f)});
                });
                registry.view<components::graphics::Sprite, components::Position, components::Transform>().each([&sprites](const auto, const auto& position, const auto& transform) {
                    sprites.emplace_back(Sprite{position.point, transform.rotation, transform.scale});
                });

                // Gather render data, if there is any
                if (render_api->dirty) { // Only data set from engine thread context needs to set dirty and be gathered here
                    if (render_api->viewport_changed) {
                        viewport = render_api->viewport();
                        projection_matrix = render_api->projectionMatrix();
                        ImGui::GetIO().DisplaySize = ImVec2(viewport.z, viewport.w);
                        render_api->viewport_changed = false;
                    }
                    render_api->dirty = false;
                }

                // Hand exclusive access back to engine
                state_sync.owner = graphics::Sync::Owner::Engine;
                lock.unlock();
                cv.notify_one();
            }
            // Now render frame from render list
            /*********************************************************************/
            EASY_BLOCK("Rendering", profiler::colors::Orange100);

            // Start the Dear ImGui frame
            {
                EASY_BLOCK("New ImGui Frame", profiler::colors::Orange200);
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplSDL2_NewFrame(render_api->window);
                ImGui::NewFrame();
            }

            glm::vec3 camera = {0.0f, 0.0f, 10.0f};
            glm::mat4 view_matrix = glm::lookAt(camera, glm::vec3{camera.x, camera.y, camera.z - 1.0f}, glm::vec3{0.0f, 1.0f, 0.0f});;
            glm::mat4 projection_view_matrix = projection_matrix * view_matrix;

            // auto frustum = math::frustum(projection_view_matrix);

            {
                EASY_BLOCK("Clearing viewport", profiler::colors::Orange200);
                glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
                glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            // Call module hook onBeforeRender before rendering the frame
            engine.callModuleHook<CM::BEFORE_RENDER>();

            // Game Rendering here
            {
                EASY_BLOCK("Rendering scene", profiler::colors::Orange200);
                run (projection_view_matrix, sprites);
            }

            // Call module hook onAfterRender after rendering the frame
            engine.callModuleHook<CM::AFTER_RENDER>();

            // Render Dear ImGUI interface
            {
                EASY_BLOCK("Rendering ImGui", profiler::colors::Orange200);
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    SDL_GL_MakeCurrent(render_api->window, render_api->gl_render_context);
                }
            }

            // End Frame
            {
                EASY_BLOCK("Swap Window", profiler::colors::DeepOrange500);
                SDL_GL_SwapWindow(render_api->window);
            }
        } while (running.load());

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

    } catch (const std::exception& e) {
        spdlog::error("Uncaught exception in graphics thread: {}", e.what());
    }

    term();

    return 0;
}

void graphics::windowChanged(gou::api::Renderer* render_api)
{
    static_cast<graphics::RenderAPI*>(render_api)->windowChanged();
}

void graphics::term (gou::api::Renderer* render_api)
{
    auto api = static_cast<graphics::RenderAPI*>(render_api);
    // Mark the thread as stoppedm_graphics_sync
    api->running.store(false);
    
    // Give the render thread exclusive access ito make sure its unblocked
    {
        std::scoped_lock<std::mutex> lock(api->state_sync.state_mutex);
        api->state_sync.owner = graphics::Sync::Owner::Renderer;
    }
    api->state_sync.sync_cv.notify_one();

    // Wait for the thread to finish
    SDL_WaitThread(api->render_thread, nullptr);
    
    // Now that the render thread is no longer accessing it, delete the render API
    delete render_api;
}
