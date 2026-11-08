#pragma once

#include <GL/freeglut.h>
#include "../ImGui/imgui_impl_glut.h"
#include <string>
#include <vector>

struct WindowDesc
{
	char name[32] = "SIM";
	int width = 1024, height = 1024;
};

class IWindowEventListener
{
public:
	virtual void onResize(int w, int h) = 0;
	virtual void onClose() = 0;
};

class IInputEventListener
{
public:
	virtual void onKeyDown(unsigned char k) = 0;
	virtual void onKeyUp(unsigned char k) = 0;
	virtual void onMouseButton(int b, int s, int x, int y) = 0;
	virtual void onMouseMove(int x, int y) = 0;
};


class Window
{
public:
	Window(WindowDesc desc);

	~Window();

	void pollEvents() { glutMainLoopEvent(); }
	bool shouldWindowClose() const { return shouldClose; }

	void addInputEventListener(IInputEventListener* listener) { inputListeners.push_back(listener); }
	void addWindowEventListener(IWindowEventListener* listener) { windowListeners.push_back(listener); }

private:
	static void s_motion(int x, int y);
	static void s_keyboard(unsigned char k, int x, int y);
	static void s_keyboardUp(unsigned char k, int x, int y);
	static void s_mouse(int b, int s, int x, int y);
	static void s_reshape(int w_, int h_);
	static void s_close();

	void motion(int x, int y);

	void keyboard(unsigned char key, int x, int y);

	void keyboardUp(unsigned char key, int x, int y);

	void mouse(int button, int state, int x, int y);

	void reshape(int w, int h);

	void close();

	int windowHandle;
	bool shouldClose = false;

	std::vector<IInputEventListener*> inputListeners;
	std::vector<IWindowEventListener*> windowListeners;
};

