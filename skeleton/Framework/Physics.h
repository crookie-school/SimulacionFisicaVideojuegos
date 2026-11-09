#pragma once

#include <PxPhysicsAPI.h>
#include <foundation/PxSimpleTypes.h>
#include <PxPhysicsVersion.h>
#include <iostream>

#define PVD_HOST "127.0.0.1" // IP localhost for PVD connection

using namespace physx;

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

class Physics : public physx::PxSimulationEventCallback
{
public:
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;
	Physics(Physics&&) = delete;
	Physics& operator=(Physics&&) = delete;


	static Physics& get()
	{
		static Physics instance;
		return instance;
	}

	void stepPhysics(double dt);

	physx::PxRigidDynamic* createDynamicRigidbody(const physx::PxTransform& transform);
	physx::PxRigidStatic* createStaticRigidbody(const physx::PxTransform& transform);

	physx::PxShape* createShape(const physx::PxGeometry& geom, physx::PxMaterial* mat);
	physx::PxMaterial* createMaterial(float staticFriction, float dynamicFriction, float restitution);

	bool raycast(const physx::PxVec3& origin, const physx::PxVec3& dir, float maxDistance, physx::PxRaycastBuffer& hitData);

private:
	Physics();
	~Physics();

	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
	void onWake(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onSleep(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
	void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);

	PxDefaultAllocator		allocator;
	PxDefaultErrorCallback	errorCallback;

	PxFoundation* foundation = NULL;
	PxPhysics* physics = NULL;


	PxMaterial* material = NULL;

	PxPvd* pvd = NULL;

	PxDefaultCpuDispatcher* dispatcher = NULL;
	PxScene* scene = NULL;
};

