#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>

#include <Geometry.h>
#include <ecsMesh.h>
#include <RenderObject.h>

using namespace GameEngine;

RenderCore::Geometry::Ptr Bullet() {
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

	return std::make_shared<RenderCore::Geometry>((RenderCore::Geometry::VertexType*)vertices.begin(), vertices.size(), (RenderCore::Geometry::IndexType*)indices.begin(), indices.size());
}

void RegisterEcsControlSystems(flecs::world& world)
{
	static auto camera_query = world.query<const CameraPtr>();
	static auto projectiles_query = world.query<const Position, const AfterbounceLifetime>();
	static auto magazine_query = world.query<Magazine>();

	world.system<AfterbounceLifetime, const BouncePlane>()
		.each([&](AfterbounceLifetime& lifetime, const BouncePlane& bounce_plance) {
		if (!lifetime.enabled && bounce_plance.ever_bounced) {
			lifetime.enabled = true;
			lifetime.current = lifetime.maximum;
		}
	});

	world.system<AfterbounceLifetime>()
		.each([&](flecs::entity e, AfterbounceLifetime& lifetime) {
		if (!lifetime.enabled) return;
		lifetime.current -= world.delta_time();
		if (lifetime.current <= 0.0) {
			// TODO
			e.disable();
			e.destruct();
			e.clear();
		}
	});

	world.system<const ControllerPtr, Position, const ShootVelocity, ReloadCooldown, ShotCooldown, Magazine>()
		.each([&](const ControllerPtr& controller, Position& pos, const ShootVelocity shoot_vel, ReloadCooldown& reload_cd, ShotCooldown& shot_cd, Magazine& magazine) {
		camera_query.each([&](const CameraPtr& camera) {
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
					Math::Vector3f bullet_direction = camera.ptr->GetViewDir();
					Math::Vector3f bullet_speed = bullet_direction * shoot_vel.value;
					//Math::Vector3f bullet_pos = bullet_direction + Math::Vector3f(pos.value.x, pos.value.y, pos.value.z);
					Math::Vector3f bullet_pos = bullet_direction + Math::Vector3f(camera.ptr->GetPosition().x, camera.ptr->GetPosition().y, camera.ptr->GetPosition().z);
					world.entity()
						.set(Position{ bullet_pos })
						.set(Velocity{ bullet_speed })
						.set(Gravity(Math::Vector3f(0.0, -9.8, 0.0)))
						.set(BouncePlane{ Math::Vector4f(0.0,1.0,0.0,5.0) })
						.set(Bounciness{ 1.0 })
						.set(AfterbounceLifetime{ 0.0, 3.0, false })
						.set(GeometryPtr{ Bullet() })
						.set(RenderObjectPtr{ new Render::RenderObject() });
				}
			}
		});
	});

	world.system<const Position>()
		.with<Destructible>()
		.each([&](flecs::entity e, const Position& pos) {
		projectiles_query.each([&](flecs::entity projectile, const Position& projectile_pos, const AfterbounceLifetime&) {
			if ((pos.value - projectile_pos.value).GetLength() < 4.0) {
				if (e.has<AdditionalAmmo>()) {
					AdditionalAmmo additional_ammo = *e.get<AdditionalAmmo>();
					magazine_query.each([&](Magazine& magazine) { magazine.current += additional_ammo.value; });
				}
				// TODO
				e.disable();
				e.destruct();
				e.clear();
				projectile.disable();
				projectile.destruct();
				projectile.clear();
			}
		});
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
		position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
		camera.ptr->SetPosition(position.value);
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([&](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.value.y = jump.value;
			}
		}
	});
}

