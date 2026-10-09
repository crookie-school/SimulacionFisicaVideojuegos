#include "Particle.h"

#include "RenderUtils.hpp"

#include <cmath>

Particle::Particle()
	:mass(1.f), pose({0.f, 0.f, 0.f}), acc({0.f, 0.f, 0.f}), lastPos(pose.p), aliveTime(0), damping(0.99f)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f), nullptr), &pose, Vector4(0.8f, 0.2f, 0.3f, 1.f));
	renderItem->addReference();
}

Particle::Particle(const Vector3D &Pos, const Vector3D &Acc, float mass, float aliveTime)
	: mass(mass), pose(Pos), vel(0, 0, 0), acc(Acc), lastPos(Pos), aliveTime(aliveTime), damping(0.99f)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f), nullptr), &pose, Vector4(0.8f, 0.2f, 0.3f, 1.f));
	renderItem->addReference();
}

Particle::Particle(const Particle& other)
	: mass(other.mass), pose(other.pose), vel(other.vel), acc(other.acc), lastPos(other.lastPos), aliveTime(other.aliveTime), damping(other.damping)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f), nullptr), &pose, Vector4(0.8f, 0.2f, 0.3f, 1.f));
	renderItem->addReference();
}

Particle& Particle::operator=(const Particle& other)
{
	if (this == &other) return *this;

	mass = other.mass;
	pose = other.pose;
	vel = other.vel;
	acc = other.acc;
	lastPos = other.lastPos;
	aliveTime = other.aliveTime;
	damping = other.damping;
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(0.5f), nullptr), &pose, Vector4(0.8f, 0.2f, 0.3f, 1.f));
	renderItem->addReference();

	return *this;
}
 
Particle::~Particle()
{
	renderItem->release();
}

void Particle::integrateEuler(double t)
{
	// Euler
	aliveTime -= t;

	pose.p = pose.p + vel * t;

	vel = vel + acc * t;

	vel = vel * std::pow(damping, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	acc = Vector3D(0, 0, 0);
}

void Particle::integrateEulerSemi(double t)
{
	// Euler semi-implicito
	aliveTime -= t;

	vel = vel + acc * t;

	vel = vel * std::pow(damping, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	pose.p = pose.p + vel * t;

	acc = Vector3D(0, 0, 0);
}

void Particle::integrateVerlet(double t)
{
	aliveTime -= t;

	const Vector3D posDelta = (2*pose.p - lastPos);

	lastPos = pose.p;

	pose.p = posDelta + acc * t * t;

	acc = Vector3D(0, 0, 0);
}
