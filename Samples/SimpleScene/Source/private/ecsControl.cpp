#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>

#include <RenderObject.h>

using namespace GameEngine;


GameEngine::RenderCore::Geometry::Ptr BulletCube()
{
	constexpr Core::array<GameEngine::RenderCore::Geometry::VertexType, 8> vertices =
	{
		Math::Vector3f(-1.0f, -1.0f, -1.0f),
		Math::Vector3f(-1.0f, +1.0f, -1.0f),
		Math::Vector3f(+1.0f, +1.0f, -1.0f),
		Math::Vector3f(+1.0f, -1.0f, -1.0f),
		Math::Vector3f(-1.0f, -1.0f, +1.0f),
		Math::Vector3f(-1.0f, +1.0f, +1.0f),
		Math::Vector3f(+1.0f, +1.0f, +1.0f),
		Math::Vector3f(+1.0f, -1.0f, +1.0f)
	};

	constexpr Core::array<GameEngine::RenderCore::Geometry::IndexType, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	return std::make_shared<GameEngine::RenderCore::Geometry>((GameEngine::RenderCore::Geometry::VertexType*)vertices.begin(), vertices.size(), (GameEngine::RenderCore::Geometry::IndexType*)indices.begin(), indices.size());
}


void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, const Speed, Gun, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, Gun& gun, const ControllerPtr& controller)
	{
		Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
		if (controller.ptr->IsPressed("GoLeft"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoRight"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoBack"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
		}
		if (controller.ptr->IsPressed("GoForward"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
		}
		position.x = position.x + currentMoveDir.Normalized().x * speed.value * world.delta_time();
		position.y = position.y + currentMoveDir.Normalized().y * speed.value * world.delta_time();
		position.z = position.z + currentMoveDir.Normalized().z * speed.value * world.delta_time();
		camera.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z));

		gun.current_cd -= world.delta_time();
		if (controller.ptr->IsPressed("Attack")) {
			if (gun.current_cd <= 0.0) {
				gun.current_cd = gun.shot_cd;
				gun.current_bullets -= 1;
				if (gun.current_bullets <= 0) {
					gun.current_bullets = gun.max_bullets;
					gun.current_cd = gun.reload_cd;
				}
				Math::Vector3f bullet_direction = camera.ptr->GetViewDir();
				Math::Vector3f bullet_speed = bullet_direction * gun.shoot_velovity;
				Math::Vector3f bullet_pos = bullet_direction + Math::Vector3f(camera.ptr->GetPosition().x, camera.ptr->GetPosition().y, camera.ptr->GetPosition().z);
				world.entity()
					.set(Position{ bullet_pos.x, bullet_pos.y, bullet_pos.z })
					.set(Velocity{ bullet_speed.x, bullet_speed.y, bullet_speed.z })
					.set(Gravity{ 0.0, -9.8, 0.0 })
					.set(BouncePlane{ 0.0,1.0,0.0,5.0 })
					.set(Bounciness{ 1.0 })
					.set(DestructionTimer{ 1.0, 1.0, false })
					.set(Collider{ 1.0, false })
					.set(EntitySystem::ECS::GeometryPtr{ BulletCube() })
					.set(EntitySystem::ECS::RenderObjectPtr{ new GameEngine::Render::RenderObject() });
			}
		}
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.y = jump.value;
			}
		}
	});
}

