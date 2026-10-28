#include "Particle.h"


#include "Render/Render.h"

#include <cmath>

Particle::Particle()
	:inverseMass(1.f), pose(0.f, 0.f, 0.f), acc(0.f, 0.f, 0.f), forceAcc(0.f, 0.f, 0.f), lastPos(pose.p), aliveTime(0), damping(0.99f)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(1.f), nullptr), &pose, Vector4(1.f, 1.f, 1.f, 1.f));
}

Particle::Particle(const Vector3D &Pos, float mass, float aliveTime, const Vector4& color, physx::PxShape* shape)
	: inverseMass(1.f/mass), pose(Pos), vel(0, 0, 0), acc(0, 0, 0), forceAcc(0.f, 0.f, 0.f), lastPos(Pos), aliveTime(aliveTime), damping(0.99f)
{
	renderItem = new RenderItem(shape, &pose, color);
}

Particle::Particle(const Particle& other)
	: inverseMass(other.inverseMass), pose(other.pose), vel(other.vel), acc(other.acc), forceAcc(other.forceAcc), lastPos(other.lastPos), aliveTime(other.aliveTime), damping(other.damping), onDeath(other.onDeath)
{
	renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(1.f), nullptr), &pose, other.renderItem->color);
}

Particle& Particle::operator=(const Particle& other)
{
	if (this == &other) return *this;

	inverseMass = other.inverseMass;
	pose = other.pose;
	vel = other.vel;
	acc = other.acc;
	lastPos = other.lastPos;
	aliveTime = other.aliveTime;
	damping = other.damping;
	onDeath = other.onDeath;
	forceAcc = other.forceAcc;
	(*renderItem) = (*other.renderItem);
	
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

	const Vector3 finalAcc = acc + forceAcc * inverseMass;

	vel = vel + finalAcc * t;

	vel = vel * std::pow(damping, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	clearForces();
}

void Particle::integrateEulerSemi(double t)
{
	// Euler semi-implicito
	aliveTime -= t;

	const Vector3 finalAcc = acc + forceAcc * inverseMass;

	vel = vel + finalAcc * t;

	vel = vel * std::pow(damping, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	pose.p = pose.p + vel * t;

	clearForces();
}

void Particle::integrateVerlet(double t)
{
	aliveTime -= t;

	const Vector3D posDelta = (2*pose.p - lastPos);

	lastPos = pose.p;

	const Vector3 finalAcc = acc + forceAcc * inverseMass;

	pose.p = posDelta + finalAcc * t * t;

	clearForces();
}

void Particle::setColor(const Vector4& col) { renderItem->color = col; }

Vector4 Particle::getColor() const { return renderItem->color; }

void Particle::setOnDeathCallback(std::function<void(Particle*)> callback)
{
	onDeath = callback;
}
