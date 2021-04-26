
#include <gou_engine.hpp>
#include "engine.hpp"

using EventPool = core::Engine::EventPool;

thread_local EventPool* g_event_pool = nullptr;

gou::events::Event* core::Engine::emit ()
{
    if (g_event_pool == nullptr) {
        // Lazy initialisation is unfortunately the only way we can initialise thread_local variables after config is read
        const std::uint32_t event_pool_size = entt::monostate<"memory/events/pool-size"_hs>();
        g_event_pool = new EventPool(event_pool_size);
        m_event_pools.push_back(g_event_pool); // Keep track of this pool so that we can gather the events into a global pool at the end of each frame
    }
    return g_event_pool->allocate();
}

// Move events from the thread local pools into the global event pool
void core::Engine::pumpEvents ()
{
    m_event_pool.reset();
    // Copy thread local events into global pool and reset thread local pools
    for (auto* pool : m_event_pools) {
        m_event_pool.copy<EventPool>(*pool);
        pool->reset();
    }
    refreshEventsIterator();
}

// Make the global event pool visible to consumers
void core::Engine::refreshEventsIterator ()
{
    // Create iterator for consumers
    m_events_iterator = {
        const_cast<gou::api::detail::EventsIterator::Type*>(m_event_pool.begin()),
        m_event_pool.count(),
    };
}

const gou::api::detail::EventsIterator& core::Engine::events ()
{
    return m_events_iterator;
}
