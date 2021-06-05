// Autogenerated, do not edit!
#include <components/core.hpp>
#include <gou/api.hpp>
#include <toml.hpp>

using namespace entt::literals;

namespace gou {
	void register_components (gou::api::Engine* engine)
	{
		entt::registry& registry = engine->registry(gou::api::Registry::Runtime);
		entt::registry& prototype_registry = engine->registry(gou::api::Registry::Prototype);
		
		registry.prepare<components::Named>();
		prototype_registry.prepare<components::Named>();
		{
			gou::api::definitions::Component component {"named"_hs, "core", "Named", entt::type_id<components::Named>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::Named>(entity, entt::hashed_string{toml::find<std::string>(table, "name").c_str()});
			};
			component.attributes.push_back({"name", gou::types::Type::HashedString, offsetof(components::Named, name)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::Named>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::Named>(entity); };
			component.size_in_bytes = sizeof(components::Named);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::Named>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::Named>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::Global>();
		prototype_registry.prepare<components::Global>();
		{
			gou::api::definitions::Component component {"global"_hs, "core", "Global", entt::type_id<components::Global>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				registry.emplace_or_replace<components::Global>(entity);
			};
			component.getter = nullptr;
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::Global>(entity); };
			component.size_in_bytes = sizeof(components::Global);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::Global>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::Global>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::Position>();
		prototype_registry.prepare<components::Position>();
		{
			gou::api::definitions::Component component {"position"_hs, "core", "Position", entt::type_id<components::Position>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto point = table.at("point");
				registry.emplace_or_replace<components::Position>(entity, glm::vec3{float(toml::find<toml::floating>(point, "x")), float(toml::find<toml::floating>(point, "y")), float(toml::find<toml::floating>(point, "z"))});
			};
			component.attributes.push_back({"point", gou::types::Type::Vec3, offsetof(components::Position, point)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::Position>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::Position>(entity); };
			component.size_in_bytes = sizeof(components::Position);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::Position>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::Position>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::Transform>();
		prototype_registry.prepare<components::Transform>();
		{
			gou::api::definitions::Component component {"transform"_hs, "core", "Transform", entt::type_id<components::Transform>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto rotation = table.at("rotation");
				auto scale = table.at("scale");
				registry.emplace_or_replace<components::Transform>(entity, glm::vec3{float(toml::find<toml::floating>(rotation, "x")), float(toml::find<toml::floating>(rotation, "y")), float(toml::find<toml::floating>(rotation, "z"))}, glm::vec3{float(toml::find<toml::floating>(scale, "x")), float(toml::find<toml::floating>(scale, "y")), float(toml::find<toml::floating>(scale, "z"))});
			};
			component.attributes.push_back({"rotation", gou::types::Type::Vec3, offsetof(components::Transform, rotation)});
			component.attributes.push_back({"scale", gou::types::Type::Vec3, offsetof(components::Transform, scale)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::Transform>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::Transform>(entity); };
			component.size_in_bytes = sizeof(components::Transform);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::Transform>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::Transform>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::Layer>();
		prototype_registry.prepare<components::graphics::Layer>();
		{
			gou::api::definitions::Component component {"layer"_hs, "graphics", "Layer", entt::type_id<components::graphics::Layer>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::graphics::Layer>(entity, std::uint8_t(toml::find<toml::integer>(table, "layer")));
			};
			component.attributes.push_back({"layer", gou::types::Type::UInt8, offsetof(components::graphics::Layer, layer)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::Layer>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::Layer>(entity); };
			component.size_in_bytes = sizeof(components::graphics::Layer);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::Layer>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::Layer>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::Sprite>();
		prototype_registry.prepare<components::graphics::Sprite>();
		{
			gou::api::definitions::Component component {"sprite"_hs, "graphics", "Sprite", entt::type_id<components::graphics::Sprite>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				registry.emplace_or_replace<components::graphics::Sprite>(entity);
			};
			component.getter = nullptr;
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::Sprite>(entity); };
			component.size_in_bytes = sizeof(components::graphics::Sprite);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::Sprite>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::Sprite>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::StaticImage>();
		prototype_registry.prepare<components::graphics::StaticImage>();
		{
			gou::api::definitions::Component component {"static-image"_hs, "graphics", "StaticImage", entt::type_id<components::graphics::StaticImage>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::graphics::StaticImage>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "image").c_str())));
			};
			component.attributes.push_back({"image", gou::types::Type::TextureResource, offsetof(components::graphics::StaticImage, image)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::StaticImage>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::StaticImage>(entity); };
			component.size_in_bytes = sizeof(components::graphics::StaticImage);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::StaticImage>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::StaticImage>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::Billboard>();
		prototype_registry.prepare<components::graphics::Billboard>();
		{
			gou::api::definitions::Component component {"billboard"_hs, "graphics", "Billboard", entt::type_id<components::graphics::Billboard>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				registry.emplace_or_replace<components::graphics::Billboard>(entity);
			};
			component.getter = nullptr;
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::Billboard>(entity); };
			component.size_in_bytes = sizeof(components::graphics::Billboard);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::Billboard>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::Billboard>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::Model>();
		prototype_registry.prepare<components::graphics::Model>();
		{
			gou::api::definitions::Component component {"model"_hs, "graphics", "Model", entt::type_id<components::graphics::Model>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::graphics::Model>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "mesh").c_str())));
			};
			component.attributes.push_back({"mesh", gou::types::Type::MeshResource, offsetof(components::graphics::Model, mesh)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::Model>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::Model>(entity); };
			component.size_in_bytes = sizeof(components::graphics::Model);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::Model>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::Model>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::Material>();
		prototype_registry.prepare<components::graphics::Material>();
		{
			gou::api::definitions::Component component {"material"_hs, "graphics", "Material", entt::type_id<components::graphics::Material>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto color = table.at("color");
				registry.emplace_or_replace<components::graphics::Material>(entity, glm::vec3{float(toml::find<toml::floating>(color, "x")), float(toml::find<toml::floating>(color, "y")), float(toml::find<toml::floating>(color, "z"))}, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "albedo").c_str())), engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "normal").c_str())), engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "metalic").c_str())), engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "roughness").c_str())), engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "ambient-occlusion").c_str())));
			};
			component.attributes.push_back({"color", gou::types::Type::RGB, offsetof(components::graphics::Material, color)});
			component.attributes.push_back({"albedo", gou::types::Type::TextureResource, offsetof(components::graphics::Material, albedo)});
			component.attributes.push_back({"normal", gou::types::Type::TextureResource, offsetof(components::graphics::Material, normal)});
			component.attributes.push_back({"metalic", gou::types::Type::TextureResource, offsetof(components::graphics::Material, metalic)});
			component.attributes.push_back({"roughness", gou::types::Type::TextureResource, offsetof(components::graphics::Material, roughness)});
			component.attributes.push_back({"ambient-occlusion", gou::types::Type::TextureResource, offsetof(components::graphics::Material, ambient_occlusion)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::Material>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::Material>(entity); };
			component.size_in_bytes = sizeof(components::graphics::Material);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::Material>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::Material>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::PointLight>();
		prototype_registry.prepare<components::graphics::PointLight>();
		{
			gou::api::definitions::Component component {"point-light"_hs, "graphics", "PointLight", entt::type_id<components::graphics::PointLight>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto color = table.at("color");
				registry.emplace_or_replace<components::graphics::PointLight>(entity, float(toml::find<toml::floating>(table, "radius")), glm::vec3{float(toml::find<toml::floating>(color, "x")), float(toml::find<toml::floating>(color, "y")), float(toml::find<toml::floating>(color, "z"))}, float(toml::find<toml::floating>(table, "intensity")));
			};
			component.attributes.push_back({"radius", gou::types::Type::Float, offsetof(components::graphics::PointLight, radius)});
			component.attributes.push_back({"color", gou::types::Type::Vec3, offsetof(components::graphics::PointLight, color)});
			component.attributes.push_back({"intensity", gou::types::Type::Float, offsetof(components::graphics::PointLight, intensity)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::PointLight>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::PointLight>(entity); };
			component.size_in_bytes = sizeof(components::graphics::PointLight);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::PointLight>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::PointLight>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::graphics::SpotLight>();
		prototype_registry.prepare<components::graphics::SpotLight>();
		{
			gou::api::definitions::Component component {"spot-light"_hs, "graphics", "SpotLight", entt::type_id<components::graphics::SpotLight>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto color = table.at("color");
				auto direction = table.at("direction");
				registry.emplace_or_replace<components::graphics::SpotLight>(entity, float(toml::find<toml::floating>(table, "range")), glm::vec3{float(toml::find<toml::floating>(color, "x")), float(toml::find<toml::floating>(color, "y")), float(toml::find<toml::floating>(color, "z"))}, glm::vec3{float(toml::find<toml::floating>(direction, "x")), float(toml::find<toml::floating>(direction, "y")), float(toml::find<toml::floating>(direction, "z"))}, float(toml::find<toml::floating>(table, "intensity")));
			};
			component.attributes.push_back({"range", gou::types::Type::Float, offsetof(components::graphics::SpotLight, range)});
			component.attributes.push_back({"color", gou::types::Type::Vec3, offsetof(components::graphics::SpotLight, color)});
			component.attributes.push_back({"direction", gou::types::Type::Vec3, offsetof(components::graphics::SpotLight, direction)});
			component.attributes.push_back({"intensity", gou::types::Type::Float, offsetof(components::graphics::SpotLight, intensity)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::graphics::SpotLight>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::graphics::SpotLight>(entity); };
			component.size_in_bytes = sizeof(components::graphics::SpotLight);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::graphics::SpotLight>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::graphics::SpotLight>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::physics::StaticBody>();
		prototype_registry.prepare<components::physics::StaticBody>();
		{
			gou::api::definitions::Component component {"static-body"_hs, "physics", "StaticBody", entt::type_id<components::physics::StaticBody>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::physics::StaticBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), nullptr);
			};
			component.attributes.push_back({"shape", gou::types::Type::Resource, offsetof(components::physics::StaticBody, shape)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::physics::StaticBody>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::physics::StaticBody>(entity); };
			component.size_in_bytes = sizeof(components::physics::StaticBody);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::physics::StaticBody>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::physics::StaticBody>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::physics::DynamicBody>();
		prototype_registry.prepare<components::physics::DynamicBody>();
		{
			gou::api::definitions::Component component {"dynamic-body"_hs, "physics", "DynamicBody", entt::type_id<components::physics::DynamicBody>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::physics::DynamicBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), float(toml::find<toml::floating>(table, "mass")), nullptr);
			};
			component.attributes.push_back({"shape", gou::types::Type::Resource, offsetof(components::physics::DynamicBody, shape)});
			component.attributes.push_back({"mass", gou::types::Type::Float, offsetof(components::physics::DynamicBody, mass)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::physics::DynamicBody>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::physics::DynamicBody>(entity); };
			component.size_in_bytes = sizeof(components::physics::DynamicBody);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::physics::DynamicBody>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::physics::DynamicBody>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::physics::KinematicBody>();
		prototype_registry.prepare<components::physics::KinematicBody>();
		{
			gou::api::definitions::Component component {"kinematic-body"_hs, "physics", "KinematicBody", entt::type_id<components::physics::KinematicBody>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::physics::KinematicBody>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), float(toml::find<toml::floating>(table, "mass")), nullptr);
			};
			component.attributes.push_back({"shape", gou::types::Type::Resource, offsetof(components::physics::KinematicBody, shape)});
			component.attributes.push_back({"mass", gou::types::Type::Float, offsetof(components::physics::KinematicBody, mass)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::physics::KinematicBody>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::physics::KinematicBody>(entity); };
			component.size_in_bytes = sizeof(components::physics::KinematicBody);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::physics::KinematicBody>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::physics::KinematicBody>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::physics::CollisionSensor>();
		prototype_registry.prepare<components::physics::CollisionSensor>();
		{
			gou::api::definitions::Component component {"collision-sensor"_hs, "physics", "CollisionSensor", entt::type_id<components::physics::CollisionSensor>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				registry.emplace_or_replace<components::physics::CollisionSensor>(entity, engine->findSignal(entt::hashed_string::value(toml::find<std::string>(table, "on-collision").c_str())));
			};
			component.attributes.push_back({"on-collision", gou::types::Type::Signal, offsetof(components::physics::CollisionSensor, on_collision)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::physics::CollisionSensor>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::physics::CollisionSensor>(entity); };
			component.size_in_bytes = sizeof(components::physics::CollisionSensor);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::physics::CollisionSensor>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::physics::CollisionSensor>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
		
		registry.prepare<components::physics::TriggerRegion>();
		prototype_registry.prepare<components::physics::TriggerRegion>();
		{
			gou::api::definitions::Component component {"trigger-region"_hs, "physics", "TriggerRegion", entt::type_id<components::physics::TriggerRegion>().seq()};
			component.loader = [](gou::api::Engine* engine, entt::registry& registry, const void* tableptr, entt::entity entity) {
				const auto& table = *reinterpret_cast<const toml::value*>(tableptr);
				auto enter_event = table.at("enter-event");
				auto exit_event = table.at("exit-event");
				registry.emplace_or_replace<components::physics::TriggerRegion>(entity, engine->findResource(entt::hashed_string::value(toml::find<std::string>(table, "shape").c_str())), gou::events::Event{entt::hashed_string::value(toml::find<std::string>(enter_event, "type").c_str()), entity, glm::vec3{float(toml::find<toml::floating>(enter_event, "x")), float(toml::find<toml::floating>(enter_event, "y")), float(toml::find<toml::floating>(enter_event, "z"))}}, gou::events::Event{entt::hashed_string::value(toml::find<std::string>(exit_event, "type").c_str()), entity, glm::vec3{float(toml::find<toml::floating>(exit_event, "x")), float(toml::find<toml::floating>(exit_event, "y")), float(toml::find<toml::floating>(exit_event, "z"))}}, engine->findSignal(entt::hashed_string::value(toml::find<std::string>(table, "on-enter").c_str())), engine->findSignal(entt::hashed_string::value(toml::find<std::string>(table, "on-exit").c_str())), std::uint32_t(toml::find<toml::integer>(table, "trigger-mask")));
			};
			component.attributes.push_back({"shape", gou::types::Type::Resource, offsetof(components::physics::TriggerRegion, shape)});
			component.attributes.push_back({"enter-event", gou::types::Type::Event, offsetof(components::physics::TriggerRegion, enter_event)});
			component.attributes.push_back({"exit-event", gou::types::Type::Event, offsetof(components::physics::TriggerRegion, exit_event)});
			component.attributes.push_back({"on-enter", gou::types::Type::Signal, offsetof(components::physics::TriggerRegion, on_enter)});
			component.attributes.push_back({"on-exit", gou::types::Type::Signal, offsetof(components::physics::TriggerRegion, on_exit)});
			component.attributes.push_back({"trigger-mask", gou::types::Type::UInt32, offsetof(components::physics::TriggerRegion, trigger_mask)});
			component.getter = [](entt::registry& registry, entt::entity entity){ return (char*)&(registry.get<components::physics::TriggerRegion>(entity)); };
			component.attached_to_entity = [](entt::registry& registry, entt::entity entity){ return registry.any_of<components::physics::TriggerRegion>(entity); };
			component.size_in_bytes = sizeof(components::physics::TriggerRegion);
			component.manage = [](entt::registry& registry, entt::entity entity, gou::api::definitions::ManageOperation op){
				switch (op) {
					case gou::api::definitions::ManageOperation::Add:
						registry.emplace_or_replace<components::physics::TriggerRegion>(entity);
						break;
					case gou::api::definitions::ManageOperation::Remove:
						registry.remove<components::physics::TriggerRegion>(entity);
						break;
					default: break;
				}
			};
			engine->registerComponent(component);
		}
	}
} // gou::