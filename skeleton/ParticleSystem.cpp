#include "ParticleSystem.h"
#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem(const Vector3D& acceleration)
	: _acceleration(acceleration)
{
}

void ParticleSystem::update(float dt)
{

	for (Particle& p : _particles) {
		p.setAcc(_acceleration);
		p.integrateEulerSemi(dt);
	}
	
	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](const Particle& p)
			{
			return !p.isAlive();
			}), 
		_particles.end());
}
