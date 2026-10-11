#pragma once
#include <vector>
#include <utility>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem(const Vector3D& acceleration);

	void update(float dt);

	template<typename ...Args>
	void createParticle(Args&&... args) {
		_particles.emplace_back(std::forward<Args>(args)...);
	}

private:
	std::vector<Particle> _particles;
	Vector3D _acceleration;
};

