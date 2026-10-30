#pragma once

#include "core.hpp"
#include "Vector3D.h"
#include "RenderUtils.hpp"
#include <functional>

class RenderItem;
class ParticleGenerator;

class Particle
{
public:
	Particle();
	Particle(const Vector3D& Pos, float mass, float aliveTime, const Vector4& color = {1.f, 1.f, 1.f, 1.f}, physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(1.f), nullptr));
	Particle(const Particle& other);
	Particle& operator=(const Particle& other);
	virtual ~Particle();
	
	virtual void integrateEuler(double t);
	virtual void integrateEulerSemi(double t);
	virtual void integrateVerlet(double t);

	bool isAlive() const { return aliveTime > 0; }
	float getLifeTime() const { return aliveTime; }
	void setLifeTime(float time) { aliveTime = time; }

	void setDamping(float d) { damping = d; }
	void setMass(float m) { m = fmax(0.1f, m); inverseMass = 1.f / m; }
	void setVel(const Vector3D &v) { vel = v; }
	void setAcc(const Vector3D &a) { acc = a; }
	void setPos(const Vector3D &p) { pose = physx::PxTransform(p); }

	float getDamping() const { return damping; }
	float getMass() const { return 1.f/inverseMass; }
	Vector3D getVel() const { return vel; }
	Vector3D getAcc() const { return acc; }
	Vector3D getPos() const { return pose.p; }

	void setColor(const Vector4& col);
	Vector4 getColor() const;

	void setOnDeathCallback(std::function<void(Particle*)> callback);
	void die() { 
		if(onDeath) onDeath(this); 
	}

	void addForce(const Vector3& force) { forceAcc += force; }
	void clearForces() { forceAcc = { 0.f, 0.f, 0.f }; }

protected:
	float inverseMass;
	Vector3D vel;
	Vector3D acc;
	physx::PxTransform pose;
	RenderItem* renderItem;
	Vector3 forceAcc;

	// Verlet integration
	Vector3D lastPos;
	float aliveTime;
	float damping;
	std::function<void(Particle*)> onDeath;
};

