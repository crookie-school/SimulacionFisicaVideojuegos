#include "Physics.h"

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
		| physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
		| physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return physx::PxFilterFlag::eDEFAULT;
}

Physics::Physics()
{
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);

	if (!foundation)
	{
		std::cerr << "PxCreateFoundation failed!" << std::endl;
		exit(1);
	}

	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);

	if (transport)
	{
		pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	}


	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);

	if (!physics)
	{
		std::cerr << "PxCreatePhysics failed!" << std::endl;
		exit(1);
	}

	PxInitExtensions(*physics, pvd);

	material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);

	dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = dispatcher;


	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = this;
	scene = physics->createScene(sceneDesc);
}

// Add custom code to the begining of the function

Physics::~Physics()
{
	// Clean scene and dispatcher first to avoid memory leaks
	if (scene)
	{
		scene->release();
		scene = nullptr;
	}
	if (dispatcher)
	{
		dispatcher->release();
		dispatcher = nullptr;
	}

	//Clean extensions before releasing physics
	PxCloseExtensions();

	// Clean material and physics
	if (material)
	{
		material->release();
		material = nullptr;
	}
	if (physics)
	{
		physics->release();
		physics = nullptr;
	}

	// Clean PVD and transport
	if (pvd)
	{
		PxPvdTransport* transport = pvd->getTransport();
		pvd->release();
		pvd = nullptr;
		if (transport)
		{
			transport->release();
		}
	}

	// Clean foundation
	if (foundation)
	{
		foundation->release();
		foundation = nullptr;
	}
}

void Physics::stepPhysics(double dt)
{
	if (!scene) return;

	// simulate() arranca la simulación de forma asíncrona en el hilo de físicas
	scene->simulate(static_cast<PxReal>(dt));

	// fetchResults(true) bloquea el hilo de renderizado hasta que la física termine.
	// En prácticas avanzadas es vital para que el renderizado no lea datos corruptos.
	scene->fetchResults(true);
}

void Physics::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	PX_UNUSED(pairs);
	PX_UNUSED(nbPairs);
	physx::PxActor* actor1 = pairHeader.actors[0];
	physx::PxActor* actor2 = pairHeader.actors[1];
}