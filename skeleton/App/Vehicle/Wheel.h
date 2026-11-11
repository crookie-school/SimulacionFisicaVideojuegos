#pragma once

#include "../../Framework/Physics.h"
#include "../../Framework/Render/Renderer.h"
#include <iostream>
#include <vector>

struct WheelDesc
{
	physx::PxVec3 wheelMountPoint, forwardDir;
	float wheelRadius, 
		suspRestLength, 
		suspStiffness, 
		suspDamping,
		rollResistance, // Resistencia al rodamiento
		tiresFrictionCoeff; // Newtons de agarre por Newtons de peso sobre la rueda
};

class Wheel
{
public:
	Wheel(PxRigidDynamic* vehicleBody, const WheelDesc& desc);

	void setMountPoint(physx::PxVec3 mount) { mountPoint = mount; }

	void applyTorque(float torque);

	void setSteer(float degrees) { steer = degrees; }

	void update(float dt);

	void render();

	void drawGui();

private:

	void applyForceToCarInMountPoint(const physx::PxVec3& localForce);

	physx::PxRigidDynamic* vehicleBody;
	physx::PxVec3 mountPoint; // Local al cuerpo del vehiculo
	physx::PxVec3 forward; // Local al cuerpo del vehiculo
	float steer = 0.f;
	float wheelRadius;
	float rollResistance;
	float tiresFrictionCoeff;
	float restLength; // k
	float stiffness; // k
	float damping; // c
	float suspensionLength;
	float prevCompression = 0.f;
	bool wasGrounded = false;

	// Gui
	std::vector<float> suspensionLengthSampels;
};

