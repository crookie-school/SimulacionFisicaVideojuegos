#pragma once

#include "core.hpp"
#include "Vector3D.h"

class RenderItem;

class Particle
{
public:
	Particle(Vector3D Pos, Vector3D Acc, float Damping);
	~Particle();
	
	void integrateEuler(double t);
	void integrateEulerSemi(double t);
	void integrateVerlet(double t);

private:
	Vector3D vel;
	Vector3D acc;
	physx::PxTransform pose;
	float dampingFactor;
	RenderItem* renderItem;

	// Verlet integration
	Vector3D lastPos;
};

