#pragma once

#include "Vector3D.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include <limits>

class ForceGenerator
{
public:
	virtual ~ForceGenerator() {}

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


class ExplosionForceGenerator : public ForceGenerator {
public:
	ExplosionForceGenerator(const Vector3D& explosionPos, float explosionStrength, float explosionRadius, float totalDuration) 
		: 
		isExploding(false),
		explosionTime(0.f), 
		totalExplosionDuration(totalDuration),
		position(explosionPos), 
		explosionStr(explosionStrength), 
		explosionRad(explosionRadius) 
	{}

	virtual void updateForce(Particle* particle, float duration) override
	{
		if (!isExploding) return;

		explosionTime += duration;
		if (explosionTime > totalExplosionDuration) isExploding = false;

		const Vector3D posToParticle = particle->getPos() - position;
		const float r = posToParticle.magnitude();
		
		if (r > explosionRad) return;


		const float timeEffect = std::exp(- explosionTime / totalExplosionDuration);
		const Vector3D force = (explosionStr / (r * r)) * posToParticle * timeEffect;

		particle->addForce(force);
	}

	virtual void updateForce(ParticleSystem& s, float d) override {
		if (!isExploding) return;
		explosionTime += d;
		if (explosionTime > totalExplosionDuration) isExploding = false;

		for (auto it = s.begin(); it != s.end(); ++it) {
			Particle* particle = *it;
			Vector3D posToParticle = particle->getPos() - position;
			const float r = posToParticle.magnitude();
			
			if (r > explosionRad) continue;

			const float timeEffect = std::exp(-explosionTime / totalExplosionDuration);

			const Vector3D force = (explosionStr / (r*r)) * posToParticle.normalized() * timeEffect;

			particle->addForce(force);
		}
		
	}

	void beginExplosion() {
		explosionTime = 0;
		isExploding = true;
	}

private:
	bool isExploding;
	float explosionTime;
	float totalExplosionDuration;
	Vector3D position;
	float explosionStr;
	float explosionRad;
};