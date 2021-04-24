#include <components/core.hpp>
#include <gou/api.hpp>
#include <toml.hpp>

using namespace entt::literals;

namespace gou {
	void register_components (gou::api::Engine* engine)
	{
		entt::registry& registry = engine->registry();
		entt::registry& prototype_registry = engine->prototypeRegistry();
		
		registry.prepare<components::Named>();
		prototype_registry.prepare<components::Named>();
		engine->registerLoader("named"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::Named>(entity, entt::hashed_string{toml::find<std::string>(table, "name").c_str()});
		});
		
		registry.prepare<components::Global>();
		prototype_registry.prepare<components::Global>();
		engine->registerLoader("global"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			registry.emplace_or_replace<components::Global>(entity);
		});
		
		registry.prepare<components::Transform>();
		prototype_registry.prepare<components::Transform>();
		engine->registerLoader("transform"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			auto rotation = table.at("rotation");
			auto scale = table.at("scale");
			registry.emplace_or_replace<components::Transform>(entity, glm::vec3{float(toml::find<toml::floating>(rotation, "x")), float(toml::find<toml::floating>(rotation, "y")), float(toml::find<toml::floating>(rotation, "z"))}, glm::vec3{float(toml::find<toml::floating>(scale, "x")), float(toml::find<toml::floating>(scale, "y")), float(toml::find<toml::floating>(scale, "z"))});
		});
		
		registry.prepare<components::Position>();
		prototype_registry.prepare<components::Position>();
		engine->registerLoader("position"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::Position>(entity, float(toml::find<toml::floating>(table, "x")), float(toml::find<toml::floating>(table, "y")), float(toml::find<toml::floating>(table, "z")));
		});
		
		registry.prepare<components::TriggerRegion>();
		prototype_registry.prepare<components::TriggerRegion>();
		engine->registerLoader("trigger-region"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			auto enter_event = table.at("enter_event");
			auto exit_event = table.at("exit_event");
			registry.emplace_or_replace<components::TriggerRegion>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), gou::events::Event{entt::hashed_string::value(toml::find<std::string>(enter_event, "type").c_str()), entity, glm::vec3{float(toml::find<toml::floating>(enter_event, "x")), float(toml::find<toml::floating>(enter_event, "y")), float(toml::find<toml::floating>(enter_event, "z"))}}, gou::events::Event{entt::hashed_string::value(toml::find<std::string>(exit_event, "type").c_str()), entity, glm::vec3{float(toml::find<toml::floating>(exit_event, "x")), float(toml::find<toml::floating>(exit_event, "y")), float(toml::find<toml::floating>(exit_event, "z"))}}, std::uint32_t(toml::find<toml::integer>(table, "trigger_mask")));
		});
		
		registry.prepare<components::TimeAware>();
		prototype_registry.prepare<components::TimeAware>();
		engine->registerLoader("time-aware"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::TimeAware>(entity, float(toml::find<toml::floating>(table, "scale_factor")));
		});
		
		registry.prepare<components::ScriptedBehavior>();
		prototype_registry.prepare<components::ScriptedBehavior>();
		engine->registerLoader("scripted-behavior"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::ScriptedBehavior>(entity, entt::hashed_string::value(toml::find<std::string>(table, "script").c_str()));
		});
		
		registry.prepare<components::graphics::Layer>();
		prototype_registry.prepare<components::graphics::Layer>();
		engine->registerLoader("layer"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::graphics::Layer>(entity, std::uint8_t(toml::find<toml::integer>(table, "layer")));
		});
		
		registry.prepare<components::graphics::Sprite>();
		prototype_registry.prepare<components::graphics::Sprite>();
		engine->registerLoader("sprite"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			registry.emplace_or_replace<components::graphics::Sprite>(entity);
		});
		
		registry.prepare<components::graphics::Billboard>();
		prototype_registry.prepare<components::graphics::Billboard>();
		engine->registerLoader("billboard"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::graphics::Billboard>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "image").c_str())));
		});
		
		registry.prepare<components::graphics::Model>();
		prototype_registry.prepare<components::graphics::Model>();
		engine->registerLoader("model"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::graphics::Model>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "mesh").c_str())), engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "material").c_str())));
		});
		
		registry.prepare<components::graphics::PointLight>();
		prototype_registry.prepare<components::graphics::PointLight>();
		engine->registerLoader("point-light"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			auto color = table.at("color");
			registry.emplace_or_replace<components::graphics::PointLight>(entity, float(toml::find<toml::floating>(table, "radius")), glm::vec3{float(toml::find<toml::floating>(color, "x")), float(toml::find<toml::floating>(color, "y")), float(toml::find<toml::floating>(color, "z"))}, float(toml::find<toml::floating>(table, "intensity")));
		});
		
		registry.prepare<components::graphics::SpotLight>();
		prototype_registry.prepare<components::graphics::SpotLight>();
		engine->registerLoader("spot-light"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			auto color = table.at("color");
			auto direction = table.at("direction");
			registry.emplace_or_replace<components::graphics::SpotLight>(entity, float(toml::find<toml::floating>(table, "range")), glm::vec3{float(toml::find<toml::floating>(color, "x")), float(toml::find<toml::floating>(color, "y")), float(toml::find<toml::floating>(color, "z"))}, glm::vec3{float(toml::find<toml::floating>(direction, "x")), float(toml::find<toml::floating>(direction, "y")), float(toml::find<toml::floating>(direction, "z"))}, float(toml::find<toml::floating>(table, "intensity")));
		});
		
		registry.prepare<components::physics::StaticBody>();
		prototype_registry.prepare<components::physics::StaticBody>();
		engine->registerLoader("static-body"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::physics::StaticBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), nullptr);
		});
		
		registry.prepare<components::physics::DynamicBody>();
		prototype_registry.prepare<components::physics::DynamicBody>();
		engine->registerLoader("dynamic-body"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::physics::DynamicBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), float(toml::find<toml::floating>(table, "mass")), nullptr);
		});
		
		registry.prepare<components::physics::KinematicBody>();
		prototype_registry.prepare<components::physics::KinematicBody>();
		engine->registerLoader("kinematic-body"_hs, [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
			const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
			registry.emplace_or_replace<components::physics::KinematicBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), float(toml::find<toml::floating>(table, "mass")), nullptr);
		});
	}
} // gou::