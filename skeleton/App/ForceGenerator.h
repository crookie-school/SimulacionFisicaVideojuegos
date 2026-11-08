#pragma once

#include "../Framework/Vector3D.h"
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
	virtual void showUI() {}
};

class GravityForceGenerator : public ForceGenerator
{
public:
	using ForceGenerator::updateForce;

	GravityForceGenerator(const Vector3D& gravity) : _g(gravity){}

	Vector3D getG() const { return _g; }
	void setG(const Vector3D& grav) { _g = grav; }

	virtual void updateForce(Particle* particle, float duration) override
	{
		particle->addForce(_g * particle->getMass());
	}

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Gravity Force Generator Settings");

		ImGui::SliderFloat3("Gravity acceleration", &_g._x, -100.f, 100.f);

		ImGui::EndGroup();
	}

private:
	Vector3D _g;
};

class WindForceGenerator : public ForceGenerator
{
public:
	using ForceGenerator::updateForce;

	WindForceGenerator(float k, const Vector3D& windVel, const Vector3D& effectVolPosition, const Vector3D& effectVolume) : _k1(k), _wV(windVel), _effectVolPos(effectVolPosition), _effectVol(effectVolume){}

	
	virtual void updateForce(Particle* particle, float duration) override
	{
		// Si la particula esta dentro del volumen de efecto
		if (isInArea(particle->getPos()))
		{
			particle->addForce(_k1 * (_wV - particle->getVel()));
		}

	}

	float getK() const { return _k1; }
	void setK(float k) { _k1 = k; }

	Vector3D getWindVel() const { return _wV; }
	void setWindVel(const Vector3D& vel) { _wV = vel; }

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Wind Force Generator Settings");

		ImGui::DragFloat("K", &_k1, 1.f, 0.f, 50.f);
		ImGui::SliderFloat3("Wind velocity", &_wV._x, -500.f, 500.f);

		ImGui::EndGroup();
	}

protected:
	bool isInArea(const Vector3D& pos) const
	{
		return
			pos.getX() > _effectVolPos.getX() && pos.getX() < _effectVolPos.getX() + _effectVol.getX() &&
			pos.getY() > _effectVolPos.getY() && pos.getY() < _effectVolPos.getY() + _effectVol.getY() &&
			pos.getZ() > _effectVolPos.getZ() && pos.getZ() < _effectVolPos.getZ() + _effectVol.getZ();
	}

	float _k1;
	Vector3D _wV;
	Vector3D _effectVolPos; // Posicion de la esquina del volumen
	Vector3D _effectVol; // Posicion del tamano del volumen
};

class TornadoForceGenerator : public WindForceGenerator
{
public:
	using ForceGenerator::updateForce;

	TornadoForceGenerator(float tornadoStrength, float centerPull, float k, const Vector3D& effectVolPosition, const Vector3D& effectVolume) :
		WindForceGenerator(k, {0.f, 0.f, 0.f}, effectVolPosition, effectVolume), tornadoStrength(tornadoStrength), centerPull(centerPull) { }

	virtual void updateForce(Particle* particle, float duration) override
	{
		const Vector3D pos = particle->getPos();
		// Si la particula esta dentro del volumen de efecto
		if (isInArea(pos))
		{
			const Vector3D center = _effectVolPos + _effectVol * 0.5f;
			const Vector3D tornadoForce = {
				-(pos.getZ() - center.getZ()),
				centerPull - (pos.getY() - center.getY()),
				pos.getX() - center.getX()
			};
			particle->addForce(tornadoStrength * tornadoForce);
		}
	}

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Tornado Force Generator Settings");

		ImGui::DragFloat("Tornado strength", &tornadoStrength, 1.f, 0.f, 100.f);
		ImGui::DragFloat("Center pull", &centerPull, 1.f, 0.f, 100.f);

		/*ImGui::Indent(5.f); La verdad es que al final no tiene mucho sentido que herede de windforce, solo usa el AABB
		WindForceGenerator::showUI();
		ImGui::Unindent(5.f);*/

		ImGui::EndGroup();
	}

private:
	float tornadoStrength;
	float centerPull;
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

	void applyTo(Particle* p)
	{
		Vector3D d = p->getPos() - position;
		const float r = d.magnitude();
		if (r > explosionRad || r < 1e-4f) return;
		const float timeEffect = std::exp(-explosionTime / totalExplosionDuration);
		p->addForce(d.normalized() * (explosionStr / (r * r)) * timeEffect);
	}

	void updateForce(Particle* p, float) override { if (isExploding) applyTo(p); }

	void updateForce(ParticleSystem& s, float dt) override
	{
		if (!isExploding) return;
		for (auto it = s.begin(); it != s.end(); ++it) applyTo(*it);
		explosionTime += dt;
		if (explosionTime > totalExplosionDuration) isExploding = false;
	}

	void beginExplosion() {
		explosionTime = 0;
		isExploding = true;
	}

	float getExplosionStr() const { return explosionStr; }
	void setExplosionStr(float str) { explosionStr = str; }
	float getExplosionRad() const { return explosionRad; }
	void setExplosionRad(float rad) { explosionRad = rad; }
	float getExplosionDuration() const { return totalExplosionDuration; }
	void setExplosionDuration(float dur) { totalExplosionDuration = dur; }

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Explosion Force Generator Settings");

		ImGui::DragFloat("Explosion strength", &explosionStr, 1.f, 0.f, 10000.f);
		ImGui::DragFloat("Explosion radius", &explosionRad, 1.f, 0.f, 100.f);
		ImGui::DragFloat("Explosion duration", &totalExplosionDuration, 1.f, 0.f, 5.f);

		ImGui::EndGroup();
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
	using ForceGenerator::updateForce;

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
	inline void setRestingLength(double newLength) { restingLength = newLength; }
	inline double getRestingLength() const { return restingLength; }

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Spring Force Generator Settings");

		ImGui::DragFloat("K", &k, 1.f, 0.f, 10.f);
		ImGui::DragFloat("Resting length", &restingLength, 1.f, 0.f, 100.f);

		ImGui::EndGroup();
	}

protected:
	float k;
	float restingLength;
	Particle* other;
};


class AnchoredSpringFG : public SpringForceGenerator
{
public:
	AnchoredSpringFG(double k, double restingLength, const Vector3D& anchorPos) :
		SpringForceGenerator(k, restingLength, new Particle(anchorPos, 1.f, 999999999.f, {0.f, 0.f, 0.f, 1.f}, physx::PxBoxGeometry(0.5f, 0.5f, 0.5f)))
	{
	}

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Anchored Spring Force Generator Settings");

		Vector3D pos = other->getPos();
		ImGui::SliderFloat3("Anchor position", &pos._x, -100.f, 100.f);
		other->setPos(pos);

		ImGui::Indent(5.f);
		SpringForceGenerator::showUI();
		ImGui::Unindent(5.f);

		ImGui::EndGroup();
	}

	void renderAnchor(Renderer& renderer)
	{
		other->render(renderer);
	}

	~AnchoredSpringFG()
	{
		delete other;
	}
};


class BouyancyForceGenerator : public ForceGenerator
{
public:
	using ForceGenerator::updateForce;

	BouyancyForceGenerator(float h, float V, float d)
		: height(h), volume(V), liquidDensity(d), liquidParticle(new Particle({0.f, h, 0.f}, 1.f, 9999999.f, {0.2f, 0.3f, 0.8f, 1.f}, physx::PxBoxGeometry(50.f, 1.f, 50.f)))
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

	void renderWater(Renderer& renderer)
	{
		liquidParticle->render(renderer);
	}

	void showUI() override
	{
		ImGui::BeginGroup();
		ImGui::Text("Bouyancy Force Generator Settings");
		
		ImGui::DragFloat("Liquid height", &height, 1.f, -50.f, 50.f);
		ImGui::DragFloat("Liquid volume", &volume, 1.f, 0.1f, 100.f);
		ImGui::DragFloat("Liquid density", &liquidDensity, 1.f, 1.f, 10000.f);
		ImGui::DragFloat("Gravity", &gravity, 1.f, 0.f, 50.f);

		ImGui::EndGroup();
	}

private:
	float height;
	float volume;
	float liquidDensity;
	float gravity = 9.8;

	Particle* liquidParticle;
};