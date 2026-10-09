#pragma once

#include "core.hpp"
#include "Vector3D.h"

class RenderItem;

class Particle
{
public:
	Particle();
	Particle(const Vector3D &Pos, const Vector3D &Acc, float mass, float aliveTime);
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
	void setMass(float m) { mass = m; }
	void setVel(const Vector3D &v) { vel = v; }
	void setAcc(const Vector3D &a) { acc = a; }
	void setPos(const Vector3D &p) { pose = physx::PxTransform(p); }

	float getDamping() const { return damping; }
	float getMass() const { return mass; }
	Vector3D getVel() const { return vel; }
	Vector3D getAcc() const { return acc; }
	Vector3D getPos() const { return pose.p; }

protected:
	float mass;
	Vector3D vel;
	Vector3D acc;
	physx::PxTransform pose;
	RenderItem* renderItem;

	// Verlet integration
	Vector3D lastPos;
	float aliveTime;
	float damping;
};

