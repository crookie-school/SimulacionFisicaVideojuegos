#pragma once

#include "Mesh.h"
#include <string>
#include <unordered_map>

class MeshLibrary {

public:
	static MeshLibrary& get() {
		static MeshLibrary instance;
		return instance;
	}

	void addMesh(const std::string& path) {
		if (!meshes.count(path)) {
			meshes[path] = Mesh(path);
		}
	}

	Mesh& getMesh(const std::string& path){
		return meshes.at(path);
	}

private:
	MeshLibrary(){}

	std::unordered_map<std::string, Mesh> meshes;
};