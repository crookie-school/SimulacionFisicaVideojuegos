#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh(const std::string& path)
{
	std::ifstream fs(path);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::attrib_t attrib;

	std::string warn, err;
	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &fs)) {
		std::cout << "Load OBJ: \n - WARN: " << warn << "\n - ERR: " << err << '\n';
	}

	for(int i = 0; i < shapes.size(); ++i)
	{
		auto& vertices = meshes[shapes[i].name];

		vertices.reserve(shapes[i].mesh.indices.size() * 3 * 2);

		for (std::size_t j = 0; j < shapes[i].mesh.indices.size(); j++) {
			int vIndex = shapes[i].mesh.indices[j].vertex_index * 3;
			int nIndex = shapes[i].mesh.indices[j].normal_index * 3;

			vertices.push_back(attrib.vertices[vIndex]);
			vertices.push_back(attrib.vertices[vIndex + 1]);
			vertices.push_back(attrib.vertices[vIndex + 2]);

			vertices.push_back(attrib.normals[nIndex]);
			vertices.push_back(attrib.normals[nIndex + 1]);
			vertices.push_back(attrib.normals[nIndex + 2]);
		}
	}
}

void Mesh::render() const {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glScaled(scale.x, scale.y, scale.z);
	for(auto& pair : meshes)
	{
		auto& vertices = pair.second;
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), vertices.data());
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), vertices.data() + 3);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void Mesh::renderModel(const std::string& name) const {

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glScaled(scale.x, scale.y, scale.z);

	auto& vertices = meshes.at(name);
	glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), vertices.data());
	glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), vertices.data() + 3);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}