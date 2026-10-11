#pragma once
#include <random>
#include <functional>
#include "Particle.h"
#include "ParticleSystem.h"

enum class GeneratorShape {Point, Box, Sphere};
enum class GeneratorVelType {Independent, Radial};

struct ParticleGenerationConfig {
	float meanMass = 1.f, desvMass = 0.f;
	Vector3D meanPose, desvPose;
	GeneratorShape shape = GeneratorShape::Point;
	GeneratorVelType velType = GeneratorVelType::Independent;
	Vector3D meanVel, desvVel; // Para VelType::Independent
	float meanSpeed = 1.f, desvSpeed = 0.f; // Para VelType::Radial
	
	float meanAliveTime = 2.f, desvAliveTime = 0.f;
	float meanDamping = 0.99f, desvDamping = 0.f;
};

class ParticleGenerator
{
public:
	ParticleGenerator(int emissionRate, const ParticleGenerationConfig& model, std::function<float(std::mt19937&)> dist = std::uniform_real_distribution<float>(-1, 1))
		: _emissionRate(emissionRate), _acc(0), _mt(), _model(model), _dist(dist)
	{
	}

	void update(ParticleSystem& particleSys, double dt)
	{
		_acc += _emissionRate * dt;
		int n = (int)_acc;
		_acc -= n;
		generate(particleSys, n);
	}

	Vector3D samplePoint()
	{
		static std::uniform_real_distribution<float> _uniform_dist(-1, 1); // Lo he puesto static para que se comparta entre todas las llamadas

		switch (_model.shape)
		{
		case GeneratorShape::Point: return _model.meanPose;
		case GeneratorShape::Box: return Vector3D{
			_model.meanPose.getX() + _uniform_dist(_mt) * _model.desvPose.getX(),
			_model.meanPose.getY() + _uniform_dist(_mt) * _model.desvPose.getY(),
			_model.meanPose.getZ() + _uniform_dist(_mt) * _model.desvPose.getZ()
		};
		case GeneratorShape::Sphere: {
			Vector3D origen = _model.meanPose;
			Vector3D desv = {
			_uniform_dist(_mt),
			_uniform_dist(_mt),
			_uniform_dist(_mt)
			};
			desv.normalize();
			desv *= _model.desvPose.magnitude() * _uniform_dist(_mt);
			return origen + desv;
		}
		}
	}

	void generate(ParticleSystem& particleSys, int amount) {
		for (int i = 0; i < amount; i++) {
			Particle particle;

			// Set pos
			Vector3D pos = samplePoint();
			particle.setPos(pos);

			// Set vel
			if(_model.velType == GeneratorVelType::Independent)
			{
				particle.setVel(randVector(_model.meanVel, _model.desvVel));
			}
			else if (_model.velType == GeneratorVelType::Radial)
			{
				Vector3D vel = (pos - _model.meanPose).normalized();
				particle.setVel(vel * (_model.meanSpeed + _dist(_mt) * _model.desvSpeed));
			}


			particle.setMass(_model.meanMass + _dist(_mt) * _model.desvMass);
			particle.setDamping(_model.meanDamping + _dist(_mt) * _model.desvDamping);
			particle.setLifeTime(_model.meanAliveTime + _dist(_mt) * _model.desvAliveTime);

			particleSys.createParticle(particle);
		}
	}

	void setPosition(const Vector3D& pos) { _model.meanPose = pos; }
	Vector3D getPosition() { return _model.meanPose; }

private:

	Vector3D randVector(const Vector3D& mean, const Vector3D& desv)
	{
		return Vector3D{
			mean.getX() + _dist(_mt) * desv.getX(),
			mean.getY() + _dist(_mt) * desv.getY(),
			mean.getZ() + _dist(_mt) * desv.getZ()
		};
	}

	float _emissionRate, _acc;
	std::mt19937 _mt;
	ParticleGenerationConfig _model;
	std::function<float(std::mt19937&)> _dist;
};

