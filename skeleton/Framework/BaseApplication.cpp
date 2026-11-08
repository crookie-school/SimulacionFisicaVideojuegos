#include "BaseApplication.h"

#include <chrono>

BaseApplication::BaseApplication(const ApplicationDesc& desc) :
	fixedTimestep(desc.fixedTimestep), window(desc.windowDesc), camera({0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}), sceneManager(&camera)
{
	window.addInputEventListener(&sceneManager);
	window.addInputEventListener(this);
	window.addWindowEventListener(this);
}

BaseApplication::~BaseApplication()
{

}

void BaseApplication::run()
{
	isRunning = true;
	std::chrono::steady_clock timer;

	auto lastFrameTime = timer.now();

	while (isRunning)
	{
		auto currentFrameTime = timer.now();
		float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;

		physicsTimeAccumulator += deltaTime;

		while (physicsTimeAccumulator >= fixedTimestep)
		{
			window.pollEvents();
			if (!isRunning) return;

			sceneManager.update(fixedTimestep);

			physicsEngine.stepPhysics(fixedTimestep);

			physicsTimeAccumulator -= fixedTimestep;
		}

		renderer.beginRender(camera);

		sceneManager.render(renderer);
		sceneManager.drawGui();
		this->drawAppUI();

		renderer.finishRender();
	}
}

void BaseApplication::onResize(int w, int h)
{
	renderer.resize(w, h);
}

void BaseApplication::onClose()
{
	isRunning = false;
}

void BaseApplication::drawAppUI(){}

void BaseApplication::onKeyDown(unsigned char k)
{
	camera.handleKey(k, 0, 0);
}

void BaseApplication::onKeyUp(unsigned char k)
{
}

void BaseApplication::onMouseButton(int b, int s, int x, int y)
{
	camera.handleMouse(b, s, x, y);
}

void BaseApplication::onMouseMove(int x, int y)
{
	camera.handleMotion(x, y);
}
