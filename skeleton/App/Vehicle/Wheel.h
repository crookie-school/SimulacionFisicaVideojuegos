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
		tiresFrictionCoeff, // Newtons de agarre por Newtons de peso sobre la rueda
		brakeCoeff;
};

class Wheel
{
public:
	Wheel(PxRigidDynamic* vehicleBody, const WheelDesc& desc);

	void setMountPoint(physx::PxVec3 mount) { mountPoint = mount; }

	void applyTorque(float torque);
	void applyBrake(float brakeStr, float dt); // brakeStr: 0 - 1

	void setSteer(float degrees) { steer = degrees; }

	void update(float dt);

	void render();

	void drawGui();

	float getWheelAngularVelocity() const;

private:
	physx::PxVec3 getMountPointGlobal() const;
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
	float brakeCoeff;
	float suspensionLength;
	float prevCompression = 0.f;
	bool isGrounded = false;
	float wheelLoad = 0.f;

	// Gui
	std::vector<float> suspensionLengthSampels;
};

