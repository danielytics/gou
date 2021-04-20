#pragma once

#include <mutex>
#include <condition_variable>

struct ImGuiContext;

namespace core {
    class Engine;
}

namespace graphics {

    struct Context;

    struct Sync {
        std::mutex state_mutex;
        std::condition_variable sync_cv;
        enum class Owner {
            Engine,
            Renderer,
        } owner = Owner::Engine;
    };

    Context* init (core::Engine&, graphics::Sync*&, ImGuiContext*&);
    void windowChanged (Context*);
    void term (Context*);

} // graphics::
