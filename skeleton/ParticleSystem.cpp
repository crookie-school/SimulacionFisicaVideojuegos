#include "ParticleSystem.h"
#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem(const Vector3D& acceleration)
	: _acceleration(acceleration)
{
}

ParticleSystem::~ParticleSystem()
{
	for (Particle* p : _particles) {
		delete p;
	}
	_particles.clear();
}

void ParticleSystem::update(float dt)
{

	for (Particle* p : _particles) {
		p->setAcc(_acceleration);
		p->integrateEulerSemi(dt);
	}
	
	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](Particle* p)
			{
				if (!p->isAlive()) {
					delete p;
					return true;
				}
				else return false;
			}), 
		_particles.end());
}
