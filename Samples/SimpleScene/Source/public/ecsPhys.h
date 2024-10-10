#pragma once

#include <flecs.h>

// TODO

struct Position
{
	float x;
	float y;
	float z;
};

struct Velocity
{
	float x;
	float y;
	float z;
};

struct Gravity
{
	float x;
	float y;
	float z;
};

struct BouncePlane
{
	float x;
	float y;
	float z;
	float w;
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

struct Speed
{
	float value;
};


struct Lifetime {
	float value;
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
	float x;
	float y;
	float z;
};