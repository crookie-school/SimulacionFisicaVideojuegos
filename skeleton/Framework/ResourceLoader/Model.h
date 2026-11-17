#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <GL/freeglut.h>
#include <PxPhysicsAPI.h>
#include "tiny_obj_loader.h"

class Model
{
public:
	Model(){}
	Model(const std::string& path);

	void render(const physx::PxVec3& scale) const;
	void renderModel(const std::string& name, const physx::PxVec3& scale) const;

	physx::PxVec3 getBoundingBox() const { return boundingBox; }

private:
	std::unordered_map<std::string, std::vector<float>> meshes;
	physx::PxVec3 boundingBox{ 0.f, 0.f, 0.f };
};

