#pragma once

#include "core.hpp"
#include "Vector3D.h"

class RenderItem;

constexpr float DAMPING = 0.99f;

class Particle
{
public:
	Particle(const Vector3D &Pos, const Vector3D &Acc, float mass);
	virtual ~Particle();
	
	virtual void integrateEuler(double t);
	virtual void integrateEulerSemi(double t);
	virtual void integrateVerlet(double t);

protected:
	float mass;
	Vector3D vel;
	Vector3D acc;
	physx::PxTransform pose;
	RenderItem* renderItem;

	// Verlet integration
	Vector3D lastPos;
};

