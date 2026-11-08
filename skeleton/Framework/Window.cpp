#include "Window.h"
#include <iostream>

Window::Window(WindowDesc desc)
{
	char* argv[] = { desc.name, nullptr };
	int argc = 1;
	glutInit(&argc, argv);

	glutInitWindowSize(desc.width, desc.height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	windowHandle = glutCreateWindow(desc.name);
	glutSetWindowData(this);

	glutDisplayFunc([]() {});
	glutKeyboardFunc(s_keyboard);
	glutKeyboardUpFunc(s_keyboardUp);
	glutMouseFunc(s_mouse);
	glutMotionFunc(s_motion);
	glutPassiveMotionFunc(s_motion);
	glutReshapeFunc(s_reshape);
	glutCloseFunc(s_close);
}

Window::~Window()
{
	if (windowHandle) glutDestroyWindow(windowHandle);
}

void Window::s_motion(int x, int y)
{
	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->motion(x, y);
}

void Window::s_keyboard(unsigned char k, int x, int y)
{
	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->keyboard(k, x, y);
}

void Window::s_keyboardUp(unsigned char k, int x, int y)
{
	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->keyboardUp(k, x, y);
}

void Window::s_mouse(int b, int s, int x, int y)
{

	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->mouse(b, s, x, y);
}

void Window::s_reshape(int w_, int h_)
{
	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->reshape(w_, h_);
}

void Window::s_close()
{
	if (auto* w = static_cast<Window*>(glutGetWindowData()))
		w->close();
}

void Window::motion(int x, int y)
{
	ImGui_ImplGLUT_MotionFunc(x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;

	for(IInputEventListener* inputListener : inputListeners) inputListener->onMouseMove(x, y);
}

void Window::keyboard(unsigned char key, int x, int y)
{
	ImGui_ImplGLUT_KeyboardFunc(key, x, y);
	if (ImGui::GetIO().WantCaptureKeyboard) return;

	for (IInputEventListener* inputListener : inputListeners) inputListener->onKeyDown(key);
}

void Window::keyboardUp(unsigned char key, int x, int y)
{
	ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);

	for (IInputEventListener* inputListener : inputListeners) inputListener->onKeyUp(key);
}

void Window::mouse(int button, int state, int x, int y)
{
	ImGui_ImplGLUT_MouseFunc(button, state, x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;

	for (IInputEventListener* inputListener : inputListeners) inputListener->onMouseButton(button, state, x, y);
}

void Window::reshape(int w, int h)
{
	std::cout << "Resize: " << w << ", " << h << '\n';
	if(ImGui::GetCurrentContext())
		ImGui_ImplGLUT_ReshapeFunc(w, h);

	for (IWindowEventListener* windowListener : windowListeners) windowListener->onResize(w, h);
}

void Window::close()
{
	shouldClose = true;
	for (IWindowEventListener* windowListener : windowListeners) windowListener->onClose();
}
