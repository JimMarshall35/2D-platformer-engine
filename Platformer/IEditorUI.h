#pragma once

class Camera2D;
class IRenderer2D;
class GLFWwindow;
class Engine;

class IEditorUserInterface {
public:
	
	virtual void DoGui() = 0;
	virtual void DrawEngineOverlay(const IRenderer2D* renderer, const Camera2D& cam) = 0;
	virtual void cursorPositionCallbackHandler(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& cam) = 0;
	virtual void scrollCallbackHandler(GLFWwindow* window, double xoffset, double yoffset, bool imGuiWantsMouse, Camera2D& cam) = 0;
	virtual void mouseButtonCallbackHandler(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& cam) = 0;
	virtual void keyboardButtonCallbackHandler(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) = 0;
	virtual void SetEngine(Engine* engine) = 0;
	virtual void frameBufferSizeChangeCallbackHandler(GLFWwindow* window, int newwidth, int newheight, Camera2D& cam) = 0;
};