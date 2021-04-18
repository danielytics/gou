#pragma once

#include <mutex>
#include <condition_variable>

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

    Context* init (core::Engine&, graphics::Sync**);
    // void update(Context*);
    void term (Context*);

} // graphics::
