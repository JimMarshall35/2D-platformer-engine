#pragma once
#include "EditorToolBase.h"
#include <vector>
#include <glm/glm.hpp>
class Line;
class DrawPolygonTool : public EditorToolBase
{
public:
	DrawPolygonTool(EditorUserInterface* ui, Engine* engine);
	// Inherited via EditorToolBase
	virtual void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override;
	virtual void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
	virtual void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override;
	virtual void drawOverlay(const IRenderer2D* renderer, const Camera2D& camera) override;
private:
	glm::vec2 GetSnappedPos();
private:
	bool _tempFinished = false;
	bool _snappedToGrid = true;
	glm::vec2 _currentMouse;
	std::vector<Line> _polyUnderConstruction;
protected:
	virtual void OnPolygonComplete();
};

