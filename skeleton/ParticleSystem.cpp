#include "ParticleSystem.h"
#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem()
{
}

ParticleSystem::~ParticleSystem()
{
	for (Particle* p : _particles) {
		delete p;
	}
	_particles.clear();
	for (Particle* p : _queueToAdd) {
		delete p;
	}
	_queueToAdd.clear();
}

void ParticleSystem::update(float dt)
{
	// Pasamos las particulas en espera a la lista de particulas al principio de los frames
	_particles.insert(_particles.end(), _queueToAdd.begin(), _queueToAdd.end());
	_queueToAdd.clear();
	
	for (Particle* p : _particles) {
		p->integrateEulerSemi(dt);
	}
	
	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](Particle* p)
			{
				if (!p->isAlive()) {
					p->die();
					delete p;
					return true;
				}
				else return false;
			}), 
		_particles.end());
}
