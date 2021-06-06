#pragma once

// From std::
#include <memory>
#include <vector>
#include <string>
#include <cstdint>

// From vendor dependencies
#include <entt/core/hashed_string.hpp>
#include <entt/core/monostate.hpp>
#include <entt/entity/registry.hpp>
#include <sparsepp/spp.h>
#include <tsl/ordered_map.h>

// Profiling
#include <easy/profiler.h>
#include <easy/arbitrary_value.h>

using namespace entt::literals;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef DEBUG_BUILD
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#include <spdlog/spdlog.h>


// From SDK
#include <gou/types.hpp>
#include <components/core.hpp>
#include <gou/helpers.hpp>


namespace gou::api::detail {
    #include <gou/type_info.hpp>
}

// From engine internal
#include "core/constants.hpp"
#include "memory/pools.hpp"
#include "utils/helpers.hpp"


// Make sure all EnTT components use the module-visible type_context
template<typename Type>
struct entt::type_seq<Type> {
    [[nodiscard]] static id_type value() ENTT_NOEXCEPT {
        static const entt::id_type value = gou::api::detail::type_context::instance()->value(entt::type_hash<Type>::value());
        return value;
    }
};
