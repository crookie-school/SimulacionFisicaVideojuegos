#pragma once

#include "Scene/SceneManager.h"
#include "Render/Camera.h"
#include "Window.h"

struct ApplicationDesc
{
	float fixedTimestep = 1.0 / 60.0;

	WindowDesc windowDesc;
};


class BaseApplication : public IWindowEventListener, public IInputEventListener
{
public:
	BaseApplication(const ApplicationDesc& desc);

	~BaseApplication();

	void run();

	void onResize(int w, int h) override;
	void onClose() override;

protected:
	bool isRunning = false;

	float physicsTimeAccumulator = 0.0;
	float fixedTimestep;
	Window window;
	SceneManager sceneManager;
	Camera camera;

	virtual void drawAppUI();

	// Inherited via IInputEventListener
	void onKeyDown(unsigned char k) override;
	void onKeyUp(unsigned char k) override;
	void onMouseButton(int b, int s, int x, int y) override;
	void onMouseMove(int x, int y) override;
};

