
#include "graphics/graphics.hpp"
#include "core/engine.hpp"

#include <SDL.h>
#include <glad/glad.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <atomic>

struct graphics::Context {
    SDL_Window* window;
    SDL_GLContext gl_context;
    SDL_GLContext init_context;
    std::atomic_bool running;
    SDL_Thread* render_thread;
    core::Engine& engine;
    graphics::Sync state_sync;
};

int render (void* data);

graphics::Context* graphics::init (core::Engine& engine, graphics::Sync*& state_sync, ImGuiContext*& imgui_context) {
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

    bool fullscreen = entt::monostate<"graphics/fullscreen"_hs>();
    auto window = SDL_CreateWindow(
        std::string(entt::monostate<"graphics/window/title"_hs>()).c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        int(entt::monostate<"graphics/resolution/width"_hs>()),
        int(entt::monostate<"graphics/resolution/height"_hs>()),
        SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

    SDL_GLContext render_context = SDL_GL_CreateContext(window);
    SDL_GLContext init_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, init_context);

    SDL_GL_SetSwapInterval(bool(entt::monostate<"graphics/v-sync"_hs>()) ? 1 : 0);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        spdlog::critical("Failed to initialize GLAD");
        return nullptr;
    }

#ifdef DEV_MODE
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

    // Create context to share with render thread and graphics API. Context is opague to the engine
    auto context = new graphics::Context{
        window,
        render_context,
        init_context,
        {true},
        nullptr,
        engine,
    };

    // Share sync object with engine
    state_sync = &context->state_sync;

    // Start render thread
    context->render_thread = SDL_CreateThread(render, "render", reinterpret_cast<void*>(context));

    return context;
}

int render (void* data) {
    using CM = gou::api::Module::CallbackMasks;

    graphics::Context* context = reinterpret_cast<graphics::Context*>(data);
    SDL_GL_MakeCurrent(context->window, context->gl_context);

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(context->window, context->gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Get context data
    auto& running = context->running;
    auto& state_sync = context->state_sync;
    auto& cv = state_sync.sync_cv;
    auto& engine = context->engine;
    //engine.setImguiContext(imgui_context);

    spdlog::info("Render thread running");
    
    // Run render loop
    do {
        /*********************************************************************/
        /* Wait for engine to hand over exclusive access to engine state.
         * Renderer will then access the ECS registry to gather all components needed for rengering,
         * accumulate a render list and hand exclusive access back to the engine. The renderer will
         * then asynchronously render from its locally owned render list.
         *********************************************************************/
        {
            // Wait for exclusive access to engine state
            std::unique_lock<std::mutex> lock(state_sync.state_mutex);
            cv.wait(lock, [&state_sync](){ return state_sync.owner == graphics::Sync::Owner::Renderer; });

            // Gather render data into render list
            // entt::registry& registry = context->engine.registry();

            // Let Dear ImGui process events from event queue
            for (const auto& event : engine.inputEvents()) {
                ImGui_ImplSDL2_ProcessEvent(&event);
            }

            // Hand exclusive access back to engine
            state_sync.owner = graphics::Sync::Owner::Engine;
            lock.unlock();
            cv.notify_one();
        }
        // Now render frame from render list
        /*********************************************************************/

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(context->window);
        ImGui::NewFrame();

        // glm::mat4 view_matrix = services::locator::camera::ref().view();
        // glm::mat4 projection_view_matrix = projection_matrix * view_matrix;

        // auto frustum = math::frustum(projection_view_matrix);

        // glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
        glViewport(0, 0, 640, 480);
        glClearColor(147.f/255.f, 237.f/255.f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Call module hook onBeforeRender before performing any game rendering
        engine.callModuleHook<CM::BEFORE_RENDER>();

        // Game Rendering here


        // Call module hook onAfterRender before ending the frame
        engine.callModuleHook<CM::AFTER_RENDER>();

        // Render Dear ImGUI interface
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // End Frame
        SDL_GL_SwapWindow(context->window);
    } while (running.load());

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void graphics::term (graphics::Context* context) {
    // Mark the thread as stopped
    context->running.store(false);
    // Unblock engine state, in case the render thread is currently waiting for it
    context->state_sync.state_mutex.unlock();
    context->state_sync.sync_cv.notify_one();
    // Wait for the thread to finish
    SDL_WaitThread(context->render_thread, nullptr);
    // Clean up OpenGL and the window
    spdlog::info("Deleting graphics context.");
    SDL_GL_DeleteContext(context->gl_context);
    SDL_GL_DeleteContext(context->init_context);
    SDL_DestroyWindow(context->window);
    delete context;
}
