#include "Particle.h"

#include "RenderUtils.hpp"

#include <cmath>

Particle::Particle(Vector3D Pos, Vector3D Acc, float Damping)
	: pose(Pos), vel(0, 0, 0), acc(Acc), dampingFactor(Damping), lastPos(Pos)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f), nullptr), &pose, Vector4(0.8f, 0.2f, 0.3f, 1.f));
	renderItem->addReference();
}

Particle::~Particle()
{
	renderItem->release();
}

void Particle::integrateEuler(double t)
{
	// Euler

	pose.p = pose.p + vel * t;

	vel = vel + acc * t;

	vel = vel * std::pow(dampingFactor, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	acc = Vector3D(0, 0, 0);
}

void Particle::integrateEulerSemi(double t)
{
	// Euler semi-implicito

	vel = vel + acc * t;

	vel = vel * std::pow(dampingFactor, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	pose.p = pose.p + vel * t;

	acc = Vector3D(0, 0, 0);
}

void Particle::integrateVerlet(double t)
{
	const Vector3D posDelta = (2*pose.p - lastPos);

	lastPos = pose.p;

	pose.p = posDelta + acc * t * t;

	acc = Vector3D(0, 0, 0);
}
