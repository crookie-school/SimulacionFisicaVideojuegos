#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <GL/freeglut.h>
#include <PxPhysicsAPI.h>
#include "tiny_obj_loader.h"

class Mesh
{
public:
	Mesh(){}
	Mesh(const std::string& path);

	void setScale(const physx::PxVec3& s) { scale = s; }
	void render() const;
	void renderModel(const std::string& name) const;

private:
	std::unordered_map<std::string, std::vector<float>> meshes;
	physx::PxVec3 scale{ 1.f, 1.f, 1.f };
};

