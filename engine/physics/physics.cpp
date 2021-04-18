
#include "physics.hpp"
#include "core/engine.hpp"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>

struct physics::Context {
    core::Engine& engine;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btDbvtBroadphase* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btCollisionDispatcher* dispatcher;
    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
};

physics::Context* physics::init (core::Engine& engine)
{
    auto context = new physics::Context{
        engine,
        new btDefaultCollisionConfiguration(),
        new btDbvtBroadphase(),
        new btSequentialImpulseConstraintSolver(),
        nullptr,
        nullptr,
        {},
    };
    context->dispatcher = new btCollisionDispatcher(context->collisionConfiguration);
    context->dynamicsWorld = new btDiscreteDynamicsWorld(context->dispatcher, context->broadphase, context->solver, context->collisionConfiguration);
    const glm::vec3& gravity = entt::monostate<"physics/gravity"_hs>();
    context->dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));

    spdlog::info("Gravity: {}, {}, {}", gravity.x, gravity.y, gravity.z);

    return context;
}

void physics::term (Context* context)
{
    if (context != nullptr) {
        // Cleanup physics engine
        if (context->dynamicsWorld)
        {
            auto dynamicsWorld = context->dynamicsWorld;
            int i;
            for (i = dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
            {
                dynamicsWorld->removeConstraint(dynamicsWorld->getConstraint(i));
            }
            for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
            {
                btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                if (body && body->getMotionState())
                {
                    delete body->getMotionState();
                }
                dynamicsWorld->removeCollisionObject(obj);
                delete obj;
            }
        }
        //delete collision shapes
        auto& collisionShapes = context->collisionShapes;
        for (int j = 0; j < collisionShapes.size(); j++)
        {
            btCollisionShape* shape = collisionShapes[j];
            delete shape;
        }
        collisionShapes.clear();
        delete context->dynamicsWorld;
        delete context->solver;
        delete context->broadphase;
        delete context->dispatcher;
        delete context->collisionConfiguration;
        delete context;
        context = nullptr;
    }
}

void physics::prepare (Context* context, entt::registry& registry)
{
    auto dynamic = registry.view<components::Position, components::physics::DynamicBody>();
    dynamic.each([context](auto entity, auto& position, auto& physics) {
        if (physics.physics_body == nullptr) {
            spdlog::warn("Creating new RigidBody: {},{},{} mass: {}", position.x, position.y, position.z, physics.mass);
            btCollisionShape* shape = new btSphereShape(1.0);
            context->collisionShapes.push_back(shape);
		    btVector3 local_inertia(0, 0, 0);
		    if (physics.mass != 0) {
                shape->calculateLocalInertia(physics.mass, local_inertia);
            }
            btQuaternion rotation;
            rotation.setEulerZYX(0, 0, 0);
            btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform{rotation, btVector3{position.x, position.y, position.z}});
            btRigidBody::btRigidBodyConstructionInfo rigitbody_info(physics.mass, motion_state, shape, local_inertia);
            rigitbody_info.m_restitution = 1.0f;
            rigitbody_info.m_friction = 0.5f;
            auto body = new btRigidBody(rigitbody_info);
            // body->setLinearVelocity(btVector3{0, 0, -5.0f});
            body->setLinearFactor(btVector3{1,1,1});
		    physics.physics_body = body;
            context->dynamicsWorld->addRigidBody(body);
        }
    });
}

void physics::simulate (Context* context)
{
    auto timeDelta = context->engine.deltaTime();
    float timestep = entt::monostate<"physics/time-step"_hs>();
    int max_substeps = entt::monostate<"physics/max-substeps"_hs>(); 
    context->dynamicsWorld->stepSimulation(timeDelta, max_substeps, timestep);
}

void physics::flush_dynamic (Context*, physics::view_flush_dynamic view)
{
    view.each([](auto entity, auto& position, const auto& body){
        auto& transform = body.physics_body->getWorldTransform();
        const auto& origin = transform.getOrigin();
        position.x = origin.x();
        position.y = origin.y();
        position.z = origin.z();
    });
}

void physics::flush_kinematic (Context*, physics::view_flush_kinematic view)
{

}
