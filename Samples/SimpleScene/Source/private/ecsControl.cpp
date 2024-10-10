#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>
//#include <GameWorld.h>
//#include <DefaultGeometry.h>
#include <Geometry.h>

#include <iostream>

using namespace GameEngine;

GeometryPtr Bullet() {
	constexpr Core::array<RenderCore::Geometry::VertexType, 8> vertices =
	{
		Math::Vector3f(-0.2f, -0.2f, -1.0f),
		Math::Vector3f(-0.2f, +0.2f, -1.0f),
		Math::Vector3f(+1.0f, +1.0f, -1.0f),
		Math::Vector3f(+1.0f, -1.0f, -1.0f),
		Math::Vector3f(-0.2f, -0.2f, +1.0f),
		Math::Vector3f(-0.2f, +0.2f, +1.0f),
		Math::Vector3f(+1.0f, +1.0f, +1.0f),
		Math::Vector3f(+1.0f, -1.0f, +1.0f)
	};

	constexpr Core::array<RenderCore::Geometry::IndexType, 36> indices =
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

	return { new RenderCore::Geometry((RenderCore::Geometry::VertexType*)vertices.begin(), vertices.size(), (RenderCore::Geometry::IndexType*)indices.begin(), indices.size()) };
}

void RegisterEcsControlSystems(flecs::world& world)
{
	// TODO

	world.system<const ControllerPtr, Position, const ShootVelocity, ReloadCooldown, ShotCooldown, Magazine>()
		.each([&](const ControllerPtr& controller, Position& pos, const ShootVelocity shoot_vel, ReloadCooldown& reload_cd, ShotCooldown& shot_cd, Magazine& magazine) {
		reload_cd.current -= world.delta_time();
		shot_cd.current -= world.delta_time();
		if (controller.ptr->IsPressed("Attack")) {
			if (reload_cd.current <= 0 && shot_cd.current <= 0) {
				shot_cd.current = shot_cd.maximum;
				magazine.current -= 1;
				if (magazine.current <= 0) {
					magazine.current = magazine.maximum;
					reload_cd.current = reload_cd.maximum;
				}
				world.entity()
					.set(Position{ pos.x + shoot_vel.x, pos.y + shoot_vel.y, pos.z + shoot_vel.z })
					.set(Velocity(shoot_vel.x, shoot_vel.y, shoot_vel.z))
					.set(Bullet())
					.set(Gravity{0.0, -9.8, 0.0})
					.set(BouncePlane{ 0.0,1.0,0.0,5.0 })
					.set(Bounciness{1.0})
					.set(Lifetime(5.0));
			}
		}
	});

	world.system<Lifetime, Position>()
		.each([&](Lifetime& lifetime, Position& position) {
			lifetime.value -= world.delta_time();
	});

	world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
		.each([&](flecs::entity e, Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
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

