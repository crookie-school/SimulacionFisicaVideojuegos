#pragma once

#include "Model.h"
#include <string>
#include <unordered_map>

class ModelLibrary {

public:
	static ModelLibrary& get() {
		static ModelLibrary instance;
		return instance;
	}

	void addModel(const std::string& path) {
		if (!models.count(path)) {
			models[path] = Model(path);
		}
	}

	Model& getModel(const std::string& path){
		return models.at(path);
	}

private:
	ModelLibrary(){}

	std::unordered_map<std::string, Model> models;
};