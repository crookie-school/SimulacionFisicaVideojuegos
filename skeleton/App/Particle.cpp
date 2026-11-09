#include "Particle.h"

#include <cmath>
#include "../Framework/Render/Renderer.h"

Particle::Particle()
	:inverseMass(1.f), pose(0.f, 0.f, 0.f), acc(0.f, 0.f, 0.f), forceAcc(0.f, 0.f, 0.f), lastPos(pose.p), aliveTime(0), damping(0.99f), geom(physx::PxSphereGeometry(1.f))
{
}

Particle::Particle(const Vector3D &Pos, float mass, float aliveTime, const physx::PxVec4& color, const physx::PxGeometryHolder& geometry)
	: inverseMass(1.f/mass), pose(Pos), vel(0, 0, 0), acc(0, 0, 0), forceAcc(0.f, 0.f, 0.f), lastPos(Pos), aliveTime(aliveTime), damping(0.99f), color(color), geom(geometry)
{
}

Particle::Particle(const Particle& other)
	: inverseMass(other.inverseMass), 
	pose(other.pose), 
	vel(other.vel), 
	acc(other.acc), 
	forceAcc(other.forceAcc), 
	lastPos(other.lastPos), 
	aliveTime(other.aliveTime), 
	damping(other.damping), 
	onDeath(other.onDeath),
	color(other.color),
	geom(other.geom)
{
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
	color = other.color;
	geom = other.geom;
	
	return *this;
}
 
Particle::~Particle()
{

}

void Particle::integrateEuler(double t)
{
	// Euler
	aliveTime -= t;

	pose.p = pose.p + vel * t;

	const Vector3D finalAcc = acc + forceAcc * inverseMass;

	vel = vel + finalAcc * t;

	vel = vel * std::pow(damping, t); // Esto es para que el damping dependa también del delta. Usamos pow porque hay que contrarestarlo de forma exponencial.

	clearForces();
}

void Particle::integrateEulerSemi(double t)
{
	// Euler semi-implicito
	aliveTime -= t;

	const Vector3D finalAcc = acc + forceAcc * inverseMass;

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

	const Vector3D finalAcc = acc + forceAcc * inverseMass;

	pose.p = posDelta + finalAcc * t * t;

	clearForces();
}

void Particle::render()
{
	Renderer::get().renderShape(geom, pose, color);
}

void Particle::setColor(const physx::PxVec4& col) { color = col; }

physx::PxVec4 Particle::getColor() const { return color; }

void Particle::setOnDeathCallback(std::function<void(Particle*)> callback)
{
	onDeath = callback;
}
