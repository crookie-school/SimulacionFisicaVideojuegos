#pragma once
#include <vector>
#include <utility>
#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem();

	void update(float dt);

	template<typename ...Args>
	void createParticle(Args&&... args) {
		particles.emplace_back(std::forward<Args>(args)...);
	}

private:
	std::vector<Particle> particles;
};

