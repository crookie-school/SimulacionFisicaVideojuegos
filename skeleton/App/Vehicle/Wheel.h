#pragma once

#include "../../Framework/Physics.h"
#include "../../Framework/Render/Renderer.h"

class Wheel
{
public:
	Wheel(PxRigidDynamic* vehicleBody, const physx::PxVec3& mountPoint, float rad, float suspRestLength, float suspStiffness, float suspDamping) :
		vehicleBody(vehicleBody), 
		mountPoint(mountPoint),
		wheelRadius(rad), 
		restLength(suspRestLength), 
		stiffness(suspStiffness), 
		damping(suspDamping),
		suspensionLength(restLength)
	{

	}

	void setMountPoint(physx::PxVec3 mount) { mountPoint = mount; }

	void update(float dt)
	{
		// Posicion de origen de la rueda (global)
		physx::PxVec3 origin = vehicleBody->getGlobalPose().transform(mountPoint);

		float raycastLenth = wheelRadius + restLength;
		physx::PxRaycastBuffer hitData;
		grounded = Physics::get().raycast(origin, { 0.f, -1.f, 0.f }, raycastLenth, hitData);

		if (!grounded)
		{
			wasGrounded = false;
			suspensionLength = restLength;
			return;
		}

		float compression = raycastLenth - hitData.block.distance;
		suspensionLength = std::fmin(std::fmax(hitData.block.distance - wheelRadius, 0.f), restLength);

		if (!wasGrounded) prevCompression = compression;
		float compressionVel = (compression - prevCompression) / dt;
		prevCompression = compression;
		wasGrounded = true;

		// F = k*x + c*v
		float force = std::fmax(0, stiffness * compression + damping * compressionVel);
		
		physx::PxRigidBodyExt::addForceAtPos(*vehicleBody, { 0.f, force, 0.f }, origin);
	}

	void render()
	{
		physx::PxVec3 wheelLocal = mountPoint + physx::PxVec3(0, -suspensionLength, 0);
		physx::PxTransform wheelGlobPos = vehicleBody->getGlobalPose() * physx::PxTransform(wheelLocal);
		Renderer::get().renderShape(physx::PxSphereGeometry(wheelRadius), wheelGlobPos, {0.f, 0.f, 0.f, 1.f});
	}

private:
	physx::PxRigidDynamic* vehicleBody;
	physx::PxVec3 mountPoint; // Local al cuerpo del vehiculo
	float wheelRadius;
	float restLength; // k
	float stiffness; // k
	float damping; // c
	float suspensionLength;
	float prevCompression = 0.f;
	bool wasGrounded = false;
	bool grounded = false;
};

