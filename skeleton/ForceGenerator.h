#pragma once

#include "Vector3D.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include <limits>
#include <algorithm>

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


class SpringForceGenerator : public ForceGenerator
{
public:
	SpringForceGenerator(double k, double restingLength, Particle* other)
		: k(k), restingLength(restingLength), other(other)
	{

	}

	virtual void updateForce(Particle* p, float d) override
	{
		Vector3D relativePosVector = other->getPos() - p->getPos();
		Vector3D force;

		const float length = relativePosVector.magnitude(); relativePosVector.normalize();
		const float delta_x = length - restingLength;

		force = relativePosVector * delta_x * k;
		
		p->addForce(force);
	}

	inline void setK(double newk) { k = newk; }
	inline double getK() const { return k; }

protected:
	double k;
	double restingLength;
	Particle* other;
};


class AnchoredSpringFG : public SpringForceGenerator
{
public:
	AnchoredSpringFG(double k, double restingLength, const Vector3& anchorPos) :
		SpringForceGenerator(k, restingLength, new Particle(anchorPos, 1.f, 999999999.f, {0.f, 0.f, 0.f, 1.f}))
	{
	}

	~AnchoredSpringFG()
	{
		delete other;
	}
};


class BouyancyForceGenerator : public ForceGenerator
{
public:
	BouyancyForceGenerator(float h, float V, float d)
		: height(h), volume(V), liquidDensity(d), liquidParticle(new Particle({0.f, h, 0.f}, 1.f, 9999999.f, {0.2f, 0.3f, 0.8f, 1.f}, CreateShape(physx::PxBoxGeometry(10.f, 1.f, 10.f))))
	{}

	~BouyancyForceGenerator(){
		delete liquidParticle;
	}

	virtual void updateForce(Particle* p, float d) override
	{
		float h = p->getPos().getY();
		float h0 = liquidParticle->getPos().getY();

		float immersed = fmin(fmax(h0 - h / height + 0.5f, 0.f), 1.f);
		Vector3D f(0, liquidDensity * volume * immersed * gravity, 0);
		
		p->addForce(f);
	}

private:
	float height;
	float volume;
	float liquidDensity;
	float gravity = 9.8;

	Particle* liquidParticle;
};