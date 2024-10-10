#pragma once

#include <flecs.h>
#include <Vector.h>

struct Position
{
	GameEngine::Math::Vector3f value;
};

struct Velocity
{
	GameEngine::Math::Vector3f value;
};

struct Gravity
{
	GameEngine::Math::Vector3f value;
};

struct BouncePlane
{
	GameEngine::Math::Vector4f value;
	bool ever_bounced = false;
};

struct Bounciness
{
	float value;
};

struct ShiverAmount
{
	float value;
};

struct FrictionAmount
{
	float value;
};

using Speed = float;

void RegisterEcsPhysSystems(flecs::world& world);



struct AfterbounceLifetime {
	float current;
	float maximum;
	bool enabled;
};

struct ShotCooldown {
	float current;
	float maximum;
};

struct ReloadCooldown {
	float current;
	float maximum;
};

struct Magazine {
	float current;
	float maximum;
};

struct ShootVelocity {
	float value;
};
