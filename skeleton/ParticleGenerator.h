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

	Vector4 meanColor = { 1.f, 1.f, 1.f, 1.f }, desvColor = { 0.f, 0.f, 0.f, 0.f };
};

class ParticleGenerator
{
public:
	ParticleGenerator(int emissionRate, const ParticleGenerationConfig& model, std::function<float(std::mt19937&)> dist = std::uniform_real_distribution<float>(-1, 1))
		: _emissionRate(emissionRate), _acc(0), _mt(), _config(model), _dist(dist), _isEmitting(true)
	{
	}

	void update(ParticleSystem& particleSys, double dt);

	Vector3D samplePoint();

	void generate(ParticleSystem& particleSys, int amount);

	void setPosition(const Vector3D& pos) { _config.meanPose = pos; }
	Vector3D getPosition() { return _config.meanPose; }

	bool isEmitting() const { return _isEmitting; }
	void setEmitting(bool emit) { _isEmitting = emit; }

	void setShape(GeneratorShape shape) { _config.shape = shape; }
	GeneratorShape getShape() const { return _config.shape; }

	void setEmissionRate(float rate) { _emissionRate = rate; }
	float getEmissionRate() const { return _emissionRate; }

	void setVelType(GeneratorVelType type) { _config.velType = type; }
	GeneratorVelType getVelType() const { return _config.velType; }

	void setVelocity(const Vector3D& vel) { _config.meanVel = vel; }
	Vector3D getVelocity() const { return _config.meanVel; }

	void setColor(const Vector4& col) { _config.meanColor = col; }
	Vector4 getColor() const { return _config.meanColor; }

private:

	Vector3D randVector(const Vector3D& mean, const Vector3D& desv)
	{
		return Vector3D{
			mean.getX() + _dist(_mt) * desv.getX(),
			mean.getY() + _dist(_mt) * desv.getY(),
			mean.getZ() + _dist(_mt) * desv.getZ()
		};
	}

	Vector4 randVector(const Vector4& mean, const Vector4& desv)
	{
		return Vector4{
			mean.x + _dist(_mt) * desv.x,
			mean.y + _dist(_mt) * desv.y,
			mean.z + _dist(_mt) * desv.z,
			mean.w + _dist(_mt) * desv.w
		};
	}

	float _emissionRate, _acc;
	bool _isEmitting;
	std::mt19937 _mt;
	ParticleGenerationConfig _config;
	std::function<float(std::mt19937&)> _dist;
};

