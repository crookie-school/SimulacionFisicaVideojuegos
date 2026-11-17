#pragma once

#include "../../Framework/Physics.h"
#include "../../Framework/Render/Renderer.h"
#include "../../Framework/ResourceLoader/ModelLibrary.h"
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
	std::string meshName;
	bool flipMesh = false;
};

class Wheel
{
public:
	Wheel(PxRigidDynamic* vehicleBody, const WheelDesc& desc);

	void setMountPoint(physx::PxVec3 mount) { mountPoint = mount; }

	void applyTorque(float torque);
	void applyBrake(float brakeStr, float dt); // brakeStr: 0 - 1

	float getSteer() const { return steer; }
	void setSteer(float degrees) { steer = degrees; }

	void update(float dt);

	void render();

	void drawGui();

	float getWheelAngularVelocity() const;

	void setFlipMesh(bool flip) { flipMesh = flip; }

	void setWheelRadius(float radius);

private:
	physx::PxVec3 getMountPointGlobal() const;
	void applyForceToCarInMountPoint(const physx::PxVec3& localForce);
	float consumeGrip(float forceDesired);

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
	float wheelGripBudget = 0.f; // Each force applied to the wheel consumes grip budget, if no budget is available, no force may be made (no budget is when wheelGripBudget == wheelLoad * tiresFrictionCoeff)
	float wheelLoad = 0.f;
	float totalRotation = 0.f; // Rotation to render the wheel rotated depending on distance translated

	// Gui
	std::vector<float> suspensionLengthSampels;

	std::string tireMeshName;
	bool flipMesh;
	physx::PxVec3 wheelScale;
};

