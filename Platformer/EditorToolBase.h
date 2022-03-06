#pragma once
#include <string>
class Camera2D;
class GLFWwindow;
class IRenderer2D;
class Engine;
class EditorUserInterface;

enum EditorToolInputRequirement : unsigned int {
	None = 0,
	CursorPositionMove = 1,
	MouseButton = 2,
	KeyboardButton = 4
};

class EditorToolBase {
public:
	std::string name;
	unsigned int InputRequirement = 0;
	virtual void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) = 0;
	virtual void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) = 0;
	virtual void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) = 0;
	virtual void drawOverlay(const IRenderer2D* renderer, const Camera2D& camera) = 0;
protected:
	Engine* _Engine;
	EditorUserInterface* _UI;
};