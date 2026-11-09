#pragma once

#include "PxPhysicsAPI.h"

#include "Camera.h"
#include <GL/freeglut.h>
#include <string>

class Renderer
{
public:
	static Renderer& get()
	{
		static Renderer instance;
		return instance;
	}

	void beginRender(const Camera& cam);
	void finishRender();

	void renderShape(const physx::PxGeometryHolder& shape, const physx::PxTransform& transform, const physx::PxVec4& color);
	void renderActor(physx::PxRigidActor* actor, bool shadows, const physx::PxVec4& color);
	void drawText(const std::string& text, int x, int y);
	void renderGeometry(const physx::PxGeometryHolder& h, bool wireframe = false);

	void resize(int w, int h);

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;
private:
	Renderer();
	~Renderer();

	void setupImGui();
	void setupDefaultRenderState();

	void startRender(const physx::PxVec3& cameraEye, const physx::PxVec3& cameraDir,
		physx::PxReal clipNear = 1.0f, physx::PxReal clipFar = 10000.0f);
};

