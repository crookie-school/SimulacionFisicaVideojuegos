#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(const std::string& path)
{
	std::ifstream fs(path);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::MaterialFileReader matReader("Assets/");
	tinyobj::attrib_t attrib;
	std::string warn, err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &fs, &matReader)) {
		std::cout << "Load OBJ: \n - WARN: " << warn << "\n - ERR: " << err << '\n';
		return;
	}

	for(int i = 0; i < shapes.size(); ++i)
	{
		auto& vertices = meshes[shapes[i].name].data;

		// <x, y, z> <nx, ny, nz> <s, t>
		vertices.reserve(shapes[i].mesh.indices.size() * 8);

		for (std::size_t j = 0; j < shapes[i].mesh.indices.size(); j++) {
			int vIndex = shapes[i].mesh.indices[j].vertex_index * 3;
			int nIndex = shapes[i].mesh.indices[j].normal_index * 3;
			int tIndex = shapes[i].mesh.indices[j].texcoord_index * 2;

			float v1 = attrib.vertices[vIndex], v2 = attrib.vertices[vIndex+1], v3 = attrib.vertices[vIndex+2];

			if (fabs(v1) > boundingBox.x) boundingBox.x = v1;
			if (fabs(v2) > boundingBox.y) boundingBox.y = v2;
			if (fabs(v3) > boundingBox.z) boundingBox.z = v3;

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);

			vertices.push_back(attrib.normals[nIndex]);
			vertices.push_back(attrib.normals[nIndex + 1]);
			vertices.push_back(attrib.normals[nIndex + 2]);

			if (tIndex < 0) {
				vertices.push_back(-1);
				vertices.push_back(-1);
			}
			else{
				vertices.push_back(attrib.texcoords[tIndex]);
				vertices.push_back(attrib.texcoords[tIndex+1]);
			}
		}

		// Solo usamos el material de la primera cara
		int matID = shapes[i].mesh.material_ids[0];
		if(matID >= 0)
			meshes[shapes[i].name].tex = materials[matID].diffuse_texname;
	}

	for (int i = 0; i < materials.size(); ++i) {
		if (materials[i].diffuse_texname.empty()) continue;

		textures.emplace(std::make_pair(materials[i].diffuse_texname, Texture(materials[i].diffuse_texname)));
	}
}

void Model::render(const physx::PxVec3& scale) const {
	glScaled(scale.x, scale.y, scale.z);
	for(auto& pair : meshes)
	{
		renderMesh(pair.second);
	}
}

void Model::renderModel(const std::string& name, const physx::PxVec3& scale) const {

	glScaled(scale.x, scale.y, scale.z);
	auto& mesh = meshes.at(name);
	renderMesh(mesh);
}

void Model::renderMesh(const Mesh& mesh) const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	const Texture* tex = nullptr;
	if (!mesh.tex.empty()) {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		tex = &textures.at(mesh.tex);
		tex->bind();
		glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(float), mesh.data.data() + 6);
	}

	glVertexPointer(3, GL_FLOAT, 8 * sizeof(float), mesh.data.data());
	glNormalPointer(GL_FLOAT, 8 * sizeof(float), mesh.data.data() + 3);
	glDrawArrays(GL_TRIANGLES, 0, mesh.data.size() / 8);

	if (!mesh.tex.empty()) {
		tex->unbind();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
