#pragma once

#include "Vector3D.h"
#include "Particle.h"
#include "ParticleSystem.h"

class ForceGenerator
{
public:
	virtual void updateForce(Particle* particle, float duration) = 0;
	virtual void updateForce(ParticleSystem& sys, float duration) {
		for (auto it = sys.begin(); it != sys.end(); ++it) {
			updateForce(*it, duration);
		}
	}
};

class GravityForceGenerator : public ForceGenerator
{
public:
	GravityForceGenerator(const Vector3D& gravity) : g(gravity){}

	virtual void updateForce(Particle* particle, float duration) override
	{
		particle->addForce(g * particle->getMass());
	}

private:
	Vector3D g;
};

class WindForceGenerator : public ForceGenerator
{
public:
	WindForceGenerator(const Vector3D& windVel, const Vector3D& effectVolPosition, const Vector3D& effectVolume) : wV(windVel), effectVolPos(effectVolPosition), effectVol(effectVolume){}

	
	virtual void updateForce(Particle* particle, float duration) override
	{
		// Si la particula esta dentro del volumen de efecto
		if (isInArea(particle->getPos()))
		{
			particle->addForce((particle->getVel() - wV));
		}

	}

protected:
	bool isInArea(const Vector3D& pos) const
	{
		return
			pos.getX() > effectVolPos.getX() && pos.getX() < effectVolPos.getX() + effectVol.getX() &&
			pos.getY() > effectVolPos.getY() && pos.getY() < effectVolPos.getY() + effectVol.getY() &&
			pos.getZ() > effectVolPos.getZ() && pos.getZ() < effectVolPos.getZ() + effectVol.getZ();
	}

	Vector3D wV;
	Vector3D effectVolPos; // Posicion de la esquina del volumen
	Vector3D effectVol; // Posicion del tamano del volumen
};

class TornadoForceGenerator : public WindForceGenerator
{
public:
	TornadoForceGenerator(float tornadoStrength, float upwardStrength, const Vector3D& effectVolPosition, const Vector3D& effectVolume) :
		WindForceGenerator({0.f, 0.f, 0.f}, effectVolPosition, effectVolume), tornadoStrength(tornadoStrength), upwardStrength(upwardStrength) { }

	virtual void updateForce(Particle* particle, float duration) override
	{
		const Vector3D pos = particle->getPos();
		// Si la particula esta dentro del volumen de efecto
		if (isInArea(pos))
		{
			const Vector3D center = effectVolPos + effectVol * 0.5f;
			const Vector3D tornadoForce = {
				-(pos.getZ() - center.getZ()),
				upwardStrength - (pos.getY() - center.getY()),
				pos.getX() - center.getX()
			};
			particle->addForce(tornadoStrength * tornadoForce);
		}
	}

private:
	float tornadoStrength;
	float upwardStrength;
};