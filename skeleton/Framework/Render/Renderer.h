#pragma once

#include "PxPhysicsAPI.h"

#include "Camera.h"
#include <GL/freeglut.h>
#include <string>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void beginRender(const Camera& cam);
	void finishRender();

	void renderShape(const physx::PxGeometryHolder& shape, const physx::PxTransform& transform, const physx::PxVec4& color);
	void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows, const physx::PxVec4& color);
	void drawText(const std::string& text, int x, int y);
	void renderGeometry(const physx::PxGeometryHolder& h, bool wireframe = false);

	void resize(int w, int h);
private:
	void setupImGui();
	void setupDefaultRenderState();

	void startRender(const physx::PxVec3& cameraEye, const physx::PxVec3& cameraDir,
		physx::PxReal clipNear = 1.0f, physx::PxReal clipFar = 10000.0f);
};

