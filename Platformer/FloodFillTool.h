#pragma once
#include "EditorToolBase.h"

class FloodFillTool : public EditorToolBase {
public:
	FloodFillTool(EditorUserInterface* ui, Engine* engine);
	//used
	void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
	//unused
	void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override {};
	void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override {};
	void drawOverlay(const IRenderer2D* renderer, const Camera2D& camera) override {};
private:
	void FloodFill();
};

