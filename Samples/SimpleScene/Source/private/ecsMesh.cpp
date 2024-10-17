#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>

#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>

using namespace GameEngine;

GameEngine::RenderCore::Geometry::Ptr Cube()
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

void RegisterEcsMeshSystems(flecs::world& world)
{
	world.system<InitComp>()
		.each([&](InitComp& init)
			{
				if (init.init == false) {
					init.init = true;
					flecs::entity cubeControl = world.entity()
						.set(Position{ -2.f, 0.f, 0.f })
						.set(Velocity{ 0.f, 0.f, 0.f })
						.set(Speed{ 10.f })
						.set(FrictionAmount{ 0.9f })
						.set(JumpSpeed{ 10.f })
						.set(Gravity{ 0.f, -9.8065f, 0.f })
						.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
						.set(Bounciness{ 0.3f })
						.set(Collider{ 10.5, false })
						.set(EntitySystem::ECS::GeometryPtr{ Cube() })
						.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
						.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

					flecs::entity cubeMoving = world.entity()
						.set(Position{ 2.f, 0.f, 0.f })
						.set(Velocity{ 0.f, 3.f, 0.f })
						.set(Gravity{ 0.f, -9.8065f, 0.f })
						.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
						.set(Bounciness{ 1.f })
						.set(DestructionTimer{ 1.0, 1.0, false })
						.set(Collider{ 10.5, false })
						.set(EntitySystem::ECS::GeometryPtr{ Cube() })
						.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });

					flecs::entity camera = world.entity()
						.set(Position{ 0.0f, 12.0f, -10.0f })
						.set(Speed{ 10.f })
						.set(CameraPtr{ Core::g_MainCamera })
						.set(Gun{ 5, 5, 1.0, 3.0, 0.0, 7.0 })
						.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

				}
			}
		);



	static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();

	world.system<EntitySystem::ECS::RenderObjectPtr, const Position>()
		.each([&](EntitySystem::ECS::RenderObjectPtr& renderObject, const Position& position)
	{
		renderObject.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z), renderThread->ptr->GetMainFrame());
	});
}


