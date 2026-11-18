#pragma once

#include "../Framework/ResourceLoader/Model.h"

struct SceneObstacle
{
	Model& model;
	physx::PxRigidActor* actor;
};