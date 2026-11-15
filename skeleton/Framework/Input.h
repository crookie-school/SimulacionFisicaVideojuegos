#pragma once

#include "Window.h"

class Input : public IInputEventListener
{
public:
	static Input& get()
	{
		static Input instance;
		return instance;
	}

	void updateInputStates()
	{
		for (int i = 0; i < 256; ++i)
		{
			prevKeyStates[i] = keyStates[i];
		}

		prevMouseL = mouseL;
		prevMouseR = mouseR;
		prevMouseM = mouseM;

		prevMouseX = mouseX; prevMouseY = mouseY;
	}

	bool isKeyDown(unsigned char k) { return keyStates[k]; }
	bool wasKeyJustPressed(unsigned char k) { return !prevKeyStates[k] && keyStates[k]; }

	enum MouseButton { MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE };
	bool isMouseButtonDown(MouseButton b)
	{
		switch (b)
		{
		case MOUSE_LEFT: return mouseL;
		case MOUSE_RIGHT: return mouseR;
		case MOUSE_MIDDLE: return mouseM;
		}
	}
	bool wasMouseButtonJustPressed(MouseButton b)
	{
		switch (b)
		{
		case MOUSE_LEFT: return !prevMouseL && mouseL;
		case MOUSE_RIGHT: return !prevMouseR && mouseR;
		case MOUSE_MIDDLE: return !prevMouseM && mouseM;
		}
	}
	void getMousePos(int& x, int& y) { x = mouseX; y = mouseY; }
	void getMouseDelta(int& dx, int& dy) { dx = mouseX - prevMouseX; dy = mouseY - prevMouseY; }

	void onKeyDown(unsigned char k) override { keyStates[k] = true; }
	void onKeyUp(unsigned char k) override { keyStates[k] = false; }
	void onMouseButton(int b, int s, int x, int y) override
	{
		switch (b)
		{
		case GLUT_LEFT_BUTTON: mouseL = !s; return;
		case GLUT_RIGHT_BUTTON: mouseR = !s; return;
		case GLUT_MIDDLE_BUTTON: mouseM = !s; return;
		}
	}
	void onMouseMove(int x, int y) override { mouseX = x; mouseY = y; }

private:
	Input()
	{
		for (int i = 0; i < 256; ++i)
		{
			keyStates[i] = false;
			prevKeyStates[i] = false;
		}
	}

	int mouseX = 0, mouseY = 0;
	int prevMouseX = 0, prevMouseY = 0;
	bool mouseL = 0, mouseR = 0, mouseM = 0;
	bool prevMouseL = 0, prevMouseR = 0, prevMouseM = 0;

	bool keyStates[256];
	bool prevKeyStates[256];
};

