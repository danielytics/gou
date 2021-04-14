#pragma once

// From std::
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <cstdint>

// From vendor dependencies
#include <entt/core/hashed_string.hpp>
#include <entt/core/monostate.hpp>
#include <entt/entity/registry.hpp>

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
#include <types.hpp>
//#include <events.hpp>
#include <utilities.hpp>

namespace gou::api::detail {
    #include <type_info.hpp>
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
