#pragma once
#include "EditorToolBase.h"
#include <vector>
class SelectTool :
    public EditorToolBase
{
public:
	SelectTool(EditorUserInterface* ui, Engine* engine);
	// used
	void handleMouseButton(int button, int action, int mods, bool imGuiWantsMouse, const Camera2D& camera) override;
	void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods, bool wantKeyboardInput) override;
	void drawOverlay(const IRenderer2D* renderer, const Camera2D& camera) override;
	//unused
	void handleMouseMove(double xpos, double ypos, bool imGuiWantsMouse, Camera2D& camera) override {};
private:
	std::vector<unsigned int> _SelectedTiles;
	unsigned int _SelectionWidth;
	std::vector<unsigned int> _ClipBoard;
	unsigned int _ClipBoardWidth;
	unsigned int _ClipBoardLayer;
	void GetNewSelection();
private:
	void CopyTiles();
	void CutTiles();
	void PasteTiles();
	void DeleteSelection();
};

