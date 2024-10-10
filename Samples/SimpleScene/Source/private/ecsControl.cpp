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

GeometryPtr Cube() {
	constexpr Core::array<RenderCore::Geometry::VertexType, 8> vertices =
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

	//world.system<const ControllerPtr, const FireRate, Velocity>()
	//	.each([&](flecs::entity e, const ControllerPtr& controller, const FireRate& fire_rate, Velocity& velocity) {
	//	if (controller.ptr->IsPressed("Attack")) {
	//		// 1
	//		//e.set(FireRate{ 2000.0 });
	//		// 2
	//		//flecs::entity new_bullet = world.entity("Gun");
	//		//new_bullet.set(FireRate{ 2000.0 });
	//		// -
	//		flecs::entity new_bullet = world.entity("Gun");
	//		///*flecs::entity new_bullet = world.entity();*/
	//		//flecs::entity new_bullet = world.entity("Gun");
	//		//new_bullet.clone();
	//		//new_bullet.set(FireRate{2.0});
	//		//flecs::entity new_bullet = world.entity("Gun");
	//		//flecs::entity new_e = e.clone(world);
	//		//new_e.set_name((std::string(e.name()) + "1").c_str());
	//		//new_e.set(FireRate{-2000.0});
	//		//e.set(FireRate{ 2000.0 });

	//		//game_world.GetCurrentLevel().GetLevelObjects();
	//	}
	//	velocity.x += fire_rate.value * 0.001;
	//});
	world.system<const ControllerPtr, const FireRate>()
		.each([&](const ControllerPtr& controller, const FireRate& firerate) {
		if (controller.ptr->IsPressed("Attack")) {
			// 1
			//e.set(FireRate{ 2000.0 });
			// 2
			//flecs::entity new_bullet = world.entity("Gun");
			//new_bullet.set(FireRate{ 2000.0 });
			// 3
			world.entity()
				.set(Position{ 3.0,0.0,0.0 })
				.set(Velocity{ 0.0,0.0,0.0 })
				.set(Speed{ 1 })
				//.set(reinterpret_cast<uint64_t>(Cube()))
				.set(Cube())
				.set(Lifetime(5.0));
			int x = 0;
			// -
			//std::string new_name = (std::string("bullet") + std::to_string(std::rand()));
			
			//new_bullet.set(ControllerPtr{"Input_default.ini"});
			//new_bullet.set( DefaultGeometry);
			//new_bullet.set(FireRate{ 5 });
		}
	});

	world.system<Lifetime, Position>()
		.each([&](Lifetime& lifetime, Position& position) {
			lifetime.value -= world.delta_time();
	});

	world.system<const ControllerPtr>()
		.each([&](const ControllerPtr& controller) {
		if (controller.ptr->IsPressed("Attack")) {
			// 1
			//e.set(FireRate{ 2000.0 });
			// 2
			//flecs::entity new_bullet = world.entity("Gun");
			//new_bullet.set(FireRate{ 2000.0 });
			// -
			std::string new_name = (std::string("bullet") + std::to_string(std::rand()));
			flecs::entity new_bullet = world.entity(new_name.c_str());
			new_bullet.set(Position{ 0.0,2.0,0.0 });
			new_bullet.set(Velocity{ 0.0,0.0,0.0 });
			new_bullet.set(Speed{ 10 });
			new_bullet.set(Bounciness{ 0.3 });
			//new_bullet.set(GeometryPtr{"Cube"});
			//new_bullet.set(ControllerPtr{"Input_default.ini"});
			//new_bullet.set( DefaultGeometry);
			new_bullet.set(FireRate{ 5 });
		}
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

