#pragma once
#include <random>
#include <functional>
#include "Particle.h"
#include "ParticleSystem.h"
#include "ImGui/imgui.h"

enum class GeneratorShape {Point, Box, Sphere, Cylinder};
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

	std::function<void(Particle*)> onDeathCallback = std::function<void(Particle*)>(nullptr);

	// Muestra un menu de configuracion para la configuracion de un generador
	void showUI()
	{
		ImGui::BeginGroup();
		ImGui::Text("Particle Generation Configuration");
		// Configurar posicion
		ImGui::SliderFloat3("Mean position", &meanPose._x, -100.f, 100.f);
		ImGui::SliderFloat3("Desv position", &desvPose._x, 0.f, 100.f);

		// Configurar la forma
		static const char* shapeTypes[] = { "Point", "Box", "Sphere", "Cylinder" };
		ImGui::Combo("Shape", (int*)&shape, shapeTypes, 4);

		// Configurar la velocidad
		int veltype = static_cast<int>(velType);
		static const char* velTypes[] = { "Independent", "Radial" };
		ImGui::Combo("Velocity type", &veltype, velTypes, 2);
		velType = (GeneratorVelType)veltype;

		if (velType == GeneratorVelType::Independent)
		{
			ImGui::SliderFloat3("Mean velocity", &meanVel._x, -500.f, 500.f);
			ImGui::SliderFloat3("Desv velocity", &desvVel._x, 0.f, 500.f);
		}
		else if (velType == GeneratorVelType::Radial)
		{
			ImGui::SliderFloat("Mean velocity", &meanSpeed, 0.f, 500.f);
			ImGui::SliderFloat("Desv velocity", &desvSpeed, 0.f, 500.f);
		}

		// Configuar el tiempo de vida
		ImGui::DragFloat("Mean alive time", &meanAliveTime, 1.f, 0.f, 10.f);
		ImGui::DragFloat("Desv alive time", &desvAliveTime, 1.f, 0.f, 10.f);

		// Configurar el damping
		ImGui::DragFloat("Mean damping", &meanDamping, 1.f, 0.1f, 1.f);
		ImGui::DragFloat("Desv damping", &desvDamping, 1.f, 0.1f, 1.f);

		// Configurar el color
		ImGui::ColorEdit4("Mean color", &meanColor.x);
		ImGui::ColorEdit4("Desv color", &desvColor.x);
		ImGui::EndGroup();
	}
};



class ParticleGenerator
{
public:
	ParticleGenerator(int emissionRate, const ParticleGenerationConfig& model, std::function<float(std::mt19937&)> dist = std::uniform_real_distribution<float>(-1, 1))
		: _emissionRate(emissionRate), _acc(0), _mt(std::random_device{}()), _config(model), _dist(dist), _isEmitting(true)
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

	void setConfig(const ParticleGenerationConfig& config) { _config = config; }

	void showUI(bool showConfigUI = false)
	{
		ImGui::BeginGroup();
		ImGui::Text("Particle Generator Configuration");

		// Activar emision
		ImGui::Checkbox("Enable emission", &_isEmitting);
		ImGui::SliderFloat("Emission rate", &_emissionRate, 1, 5000);

		// Limitado pero bueno, sirve para probar en las demos
		const char* distTypes[] = {"Uniform", "Normal"};
		if (ImGui::Combo("Distribution", &_distChosen, distTypes, 2))
		{
			if (_distChosen == 0)
				_dist = std::uniform_real_distribution<float>(-1, 1);
			else
				_dist = std::normal_distribution<float>(0, 1);
		}

		if(showConfigUI) _config.showUI();

		ImGui::EndGroup();
	}

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
	std::function<float(std::mt19937&)> _dist; int _distChosen = 0; // (0 uniform, 1 normal) Esto no muestra la distribucion correct si se pasa una por el codigo distinta pero no se me ocurre como hacerlo
};

