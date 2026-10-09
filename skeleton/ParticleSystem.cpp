#include "ParticleSystem.h"
#include <algorithm>

ParticleSystem::ParticleSystem()
{
}

void ParticleSystem::update(float dt)
{

	for (Particle& p : particles) {
		p.integrateEulerSemi(dt);
	}
	
	particles.erase(
		std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
			return !p.isAlive();
			}), 
		particles.end());
}
