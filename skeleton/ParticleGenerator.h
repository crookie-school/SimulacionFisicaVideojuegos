#pragma once
#include <random>
#include <functional>
#include "Particle.h"
#include "ParticleSystem.h"

struct ParticleGenerationModel {
	Particle median;
	Particle variance;
};

template <typename distributionT>
class ParticleGenerator
{
public:
	ParticleGenerator(const ParticleGenerationModel& model)
		: _mt(), _model(model), _dist(-1.f, 1.f)
	{
	}

	void generate(ParticleSystem& particleSys, int amount) {
		for (int i = 0; i < amount; i++) {
			Particle particle;
			particle.setVel(_model.median.getVel() + _dist(_mt) * _model.variance.getVel());
			particle.setAcc(_model.median.getAcc() + _dist(_mt) * _model.variance.getAcc());
			particle.setMass(_model.median.getMass() + _dist(_mt) * _model.variance.getMass());
			particle.setPos(_model.median.getPos() + _dist(_mt) * _model.variance.getPos());
			particle.setDamping(_model.median.getDamping() + _dist(_mt) * _model.variance.getDamping());
			particle.setLifeTime(_model.median.getLifeTime() + _dist(_mt) * _model.variance.getLifeTime());

			particleSys.createParticle(particle);
		}
	}

private:
	std::mt19937 _mt;
	ParticleGenerationModel _model;
	distributionT _dist;
};

