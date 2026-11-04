#include "ParticleGenerator.h"

void ParticleGenerator::update(ParticleSystem& particleSys, double dt)
{
	if (!_isEmitting) return;

	_acc += _emissionRate * dt;
	int n = (int)_acc;
	_acc -= n;
	generate(particleSys, n);
}

Vector3D ParticleGenerator::samplePoint()
{
	switch (_config.shape)
	{
	case GeneratorShape::Point: return _config.meanPose;
	case GeneratorShape::Box: return Vector3D{
		_config.meanPose.getX() + _dist(_mt) * _config.desvPose.getX(),
		_config.meanPose.getY() + _dist(_mt) * _config.desvPose.getY(),
		_config.meanPose.getZ() + _dist(_mt) * _config.desvPose.getZ()
	};
	case GeneratorShape::Sphere: {
		Vector3D origen = _config.meanPose;
		Vector3D desv = {
			_dist(_mt),
			_dist(_mt),
			_dist(_mt)
		};
		desv.normalize();
		desv *= _config.desvPose.magnitude() * _dist(_mt);
		return origen + desv;
	}
	case GeneratorShape::Cylinder: {
		Vector3D origen = _config.meanPose;
		Vector3D desv = {
			_dist(_mt),
			0,
			_dist(_mt)
		};
		desv.normalize();
		desv *= _config.desvPose.magnitude() * _dist(_mt);
		desv.setY(_dist(_mt) * _config.desvPose.getY() * 2);
		return origen + desv;
	}
	}
}

void ParticleGenerator::generate(ParticleSystem& particleSys, int amount)
{

	for (int i = 0; i < amount; i++)
	{
		Particle particle;

		// Set pos
		Vector3D pos = samplePoint();
		particle.setPos(pos);

		// Set vel
		if (_config.velType == GeneratorVelType::Independent)
		{
			particle.setVel(randVector(_config.meanVel, _config.desvVel));
		}
		else if (_config.velType == GeneratorVelType::Radial)
		{
			Vector3D vel;
			if (_config.shape == GeneratorShape::Point) vel = Vector3D( 0.f, 0.f, 0.f );
			else vel = (pos - _config.meanPose).normalized();

			particle.setVel(vel * (_config.meanSpeed + _dist(_mt) * _config.desvSpeed));
		}


		particle.setMass(_config.meanMass + _dist(_mt) * _config.desvMass);
		particle.setDamping(_config.meanDamping + _dist(_mt) * _config.desvDamping);
		particle.setLifeTime(_config.meanAliveTime + _dist(_mt) * _config.desvAliveTime);

		particle.setColor(randVector(_config.meanColor, _config.desvColor));

		particle.setOnDeathCallback(_config.onDeathCallback);

		particleSys.createParticle(particle);
	}
}
