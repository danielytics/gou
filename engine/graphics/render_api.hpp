#pragma once

#include <gou/api.hpp>

#include <SDL.h>
#include <glad/glad.h>

#include <imgui.h>

#include "graphics.hpp"

namespace core {
    class Engine;
}

namespace graphics {

    /*
     * This class is the API provided for other parts of the engine (especially modules) to interact with
     * the graphics subsystem. It is also the container for internal context data for.
     * It is not the actual renderer itself, as that runs in the graphics thread while this class is meant
     * to be called by the engine threads and only called by the graphics thread when the state_sync mutex
     * is held.
     * This is slightly confusing, maybe it can be refactored later, but it ensures that methods don't get
     * called by the wrong threads at the wrong time.
     */
    class RenderAPI : public gou::api::Renderer
    {
    public:
        RenderAPI (core::Engine& e) : engine(e) {}
        virtual ~RenderAPI();

        core::Engine& engine;
        SDL_Window* window;
        SDL_GLContext gl_render_context;
        SDL_GLContext gl_init_context;
        std::atomic_bool running = true;
        SDL_Thread* render_thread;
        Sync state_sync;
        bool dirty = false;
        bool resolution_changed = false;

        /**********************************************************************
         * Below functions must hold the state_sync lock
         */ 
        void setViewport (const glm::vec4& viewport) final;
        const glm::vec4& viewport () const { return m_viewport; }

        const glm::mat4& projectionMatrix () const { return m_projection_matrix; }

        void windowChanged ();

        /**********************************************************************
         * Below functions should only be used in render thread context
         */

    private:
        glm::vec4 m_viewport;
        glm::mat4 m_projection_matrix;
    };

} // graphics::
