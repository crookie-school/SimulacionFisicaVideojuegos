#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Texture.h"
#include <GL/freeglut.h>
#include <PxPhysicsAPI.h>
#include "tiny_obj_loader.h"

struct Mesh {
	std::vector<float> data;
	std::string tex;
};

class Model
{
public:
	Model(){}
	Model(const std::string& path);

	void render(const physx::PxVec3& scale) const;
	void renderModel(const std::string& name, const physx::PxVec3& scale) const;

	physx::PxVec3 getBoundingBox() const { return boundingBox; }

private:
	void renderMesh(const Mesh& mesh) const;

	std::unordered_map<std::string, Mesh> meshes;
	std::unordered_map<std::string, Texture> textures;

	physx::PxVec3 boundingBox{ 0.f, 0.f, 0.f };
};

