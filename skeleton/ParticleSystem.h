#pragma once
#include <vector>
#include <utility>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem(const Vector3D& acceleration);
	~ParticleSystem();

	void setAcceleration(const Vector3D& acc) {
		_acceleration = acc;
	}

	void update(float dt);

	template<typename ...Args>
	void createParticle(Args&&... args) {
		_queueToAdd.emplace_back(new Particle(std::forward<Args>(args)...));
	}

	std::vector<Particle*>::iterator begin() { return _particles.begin(); }
	std::vector<Particle*>::iterator end() { return _particles.end(); }

private:
	std::vector<Particle*> _queueToAdd;
	std::vector<Particle*> _particles;
	Vector3D _acceleration;
};

